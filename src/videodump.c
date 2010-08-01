/*
GPL. blah, blah, blah.
*/

#include "videodump.h"
#include "lib/framework/types.h"
#include "lib/ivis_common/pietypes.h"
#include "src/console.h"
#include <GL/gl.h>
#include <physfs.h>


/* video dump stuff */

extern char VideoDumpPath[];  // over in main.c

/*  screen.c definitions */
extern SDL_Surface	*screen;



BOOL videodump_required = false;


static int sample_rate = 48000;
static int sample_size = 4; // 16 bit stereo  sb16le
static int max_duration = 2; // seconds
static int aud_sample_count;

static ALCuint audio_frequency= 48000;
static ALCenum audio_format = AL_FORMAT_STEREO16;
static ALCsizei aud_buffersize = 192000;
static ALCdevice *capture_device = NULL;
static unsigned int max_frames = 999;
static unsigned int scene_num = 0;
static unsigned int frame_num = 0;
static const unsigned int channelsPerPixel = 3;

static iV_Image vid_image = { 0, 0, 0, NULL };
static PHYSFS_File* video_file;
static PHYSFS_File* audio_file;

static char video_filename[PATH_MAX];
static char audio_filename[PATH_MAX];

static unsigned char *vid_tmp_buf = 0;
static unsigned char *aud_tmp_buf = 0;





/***
* Dump the current screen to disk as a video frame.
* The videodump_required flag is controlled by videoDumpToDisk()
* Bytes from glReadPixels are written as a PPM file.  This is postprocessed
* into a movie format like AVI with something like ffmpeg or mencoder
* or our movie_tools/wzencode.py script 
*/

void videoDoDumpToDiskIfRequired(void)
{
	char buffer[80];  // for ppm file header 
	const char *fileName = video_filename;
//	const char *audiofilename = audio_filename;


	if (!videodump_required) return;

	debug( LOG_3D, "Saving video frame %s\n", fileName );

	// max vid frames exceeded?
	++frame_num;
	if( frame_num > max_frames )
	{
		videodump_required = false;
		// fixme: msg here
		printf("\n*** video dump stopped. videodump_max_frames reached ***\n");
		return;
	}

#if 0
//fixme: replace with check for scene:  wz2100_001_001.png

	if( PHYSFS_exists( videodump_filename))
	{ // oops! file exists.  don't overwrite
		videodump_required = false;
		// fixme: msg here
		return;
	}
#endif

	// Dump the currently displayed screen in a buffer

	glReadPixels(0, 0, vid_image.width, vid_image.height,
				 GL_RGB, GL_UNSIGNED_BYTE, vid_image.bmp);

	/* writing a png takes too long so let's dump a binary PPM
	   PPM format is an ASCII header followed by raw bytes from top to bottom.
	   Since glReadPixels reads bottom up, we need to flip the image.
	*/

	{
		/* flip image vertically by copying rows from top of vid_image 
		   to bottom of tmp_buf
		*/

		// image buffers are alloced and freed in videoDumpToDisk()
		if( vid_image.bmp && vid_tmp_buf) {
			int row;
			unsigned char *top;
			unsigned char *bottom;
			int row_stride = vid_image.width * channelsPerPixel;

			top = vid_image.bmp;
			bottom = vid_tmp_buf + row_stride * (vid_image.height - 1);

			for( row = 0; row < vid_image.height; 
				 row ++, top += row_stride, bottom -= row_stride) {

				memcpy( bottom, top, row_stride);  // copy top to bottom
			}
			 
		} else {
			// half-hearted error msg
			printf( "error: malloc failed in video dump\n");  
			return;
		}
	}

	// grab available sound samples
	alcGetIntegerv( capture_device, 
					ALC_CAPTURE_SAMPLES, 
					sizeof( aud_sample_count), 
					&aud_sample_count);
	//printf("audio samples available %d\n", aud_sample_count);

	if(aud_sample_count){
		alcCaptureSamples( capture_device, 
						   aud_tmp_buf, 
						   aud_sample_count);
		// write raw audio
		PHYSFS_write( audio_file, aud_tmp_buf, aud_sample_count * 4, 1);

	}


	// write ppm header: magic number width height, max value
	sprintf( buffer, "P6\n%d %d\n255\n", 
			 vid_image.width, 
			 vid_image.height );
	PHYSFS_write( video_file,  buffer, strlen( buffer ), 1 );

	// write vidoe data
	PHYSFS_write( video_file, vid_tmp_buf, 
				  vid_image.width * vid_image.height * channelsPerPixel , 1);

}






/** videoDumpToDisk
 *  Controls dumping screens to video frames.
 *  Each dump gets a new sequence number
 *  
 *  toggles flag for videoDumpIfRequired
 *  increments scene number
 *  resets frame counter
 *
 *  \param path The directory path to save the screenshot in.
 */
void videoDumpToDisk(const char* path)
{

	/* 
	   Toggle video dump state
	   if we are starting a new dump:
	       increment the scene number.
		     scene numbers run from 1..N
		     frame numbers run from 1..N
		   turn on audio capture
	*/
	
	if(videodump_required){  // was on.  turn off
		videodump_required = false;
		// do cleanup
		if(vid_image.bmp){
			free( vid_image.bmp );
			vid_image.bmp=NULL;
			vid_image.width = 0; 
			vid_image.height = 0;
		}
		if(vid_tmp_buf){
			free( vid_tmp_buf);
			vid_tmp_buf = NULL;
		}

		if( aud_tmp_buf ){
			free( aud_tmp_buf );
			aud_tmp_buf = NULL;
		}

		// close our files
		PHYSFS_close( video_file);
		PHYSFS_close( audio_file);

		alcCaptureCloseDevice( capture_device );


#if 0
//no audio capture right now
        // stop audio capture
		soundCapture( false );
#endif

	} 
	else {  // turn on
		/* increment the scene number and check for overflow so 
		   we don't overwrite files.
		   set up our buffers.
		   open the file.
		*/
		++scene_num;

		if( scene_num == 0){  // overflow!
			videodump_required = false;
			CONPRINTF( ConsoleString, 
					   (ConsoleString, "Oops! Scene Number overflow"));
			return;
		}
		videodump_required=true;
		frame_num=0;
		// do setup
		ASSERT(screen->w >= 0 && screen->h >= 0, 
			   "Screen has negative dimensions! Width = %d; Height = %d", 
			   screen->w, screen->h);

		vid_image.width = screen->w;
		vid_image.height = screen->h;
		vid_image.bmp = malloc(channelsPerPixel * vid_image.width * vid_image.height);
		if (vid_image.bmp == NULL)
		{
			vid_image.width = 0; vid_image.height = 0;
			debug(LOG_ERROR, "Couldn't allocate memory");
			return;
		}
		vid_tmp_buf = (unsigned char*) malloc( vid_image.width * vid_image.height * channelsPerPixel );
		if( !vid_tmp_buf){
			debug(LOG_ERROR, "could not allocate memory for video tmp buf");
			return;
		}

		aud_tmp_buf = (unsigned char*) malloc( sample_rate * sample_size * max_duration);

		// open our frame file.
		// filename is   dir/wz2100_scene.ppm
		ssprintf( video_filename, "%s/wz2100_%03d.ppm", 
				  VideoDumpPath, 
				  scene_num );

		ssprintf( audio_filename, "%s/wz2100_%03d.raw", 
			  VideoDumpPath, 
				  scene_num );

		if((video_file = PHYSFS_openWrite(video_filename)) == NULL)
		{
			debug(LOG_ERROR, 
				  "videodump: PHYSFS_openWrite failed. file %s error: %s\n", 
				  video_filename, PHYSFS_getLastError());
			return;
		}

		if((audio_file = PHYSFS_openWrite(audio_filename)) == NULL)
		{
			debug(LOG_ERROR, 
				  "videodump: PHYSFS_openWrite failed. file %s error: %s\n", 
				  audio_filename, PHYSFS_getLastError());
			return;
		}

		// open our capture device
		capture_device = alcCaptureOpenDevice(NULL,
											  audio_frequency,
											  audio_format,
											  aud_buffersize);
		if(!capture_device ){
			printf("\n** failed to open capture device **\n");
			return;
		}

		alGetError();
		alcCaptureStart(capture_device);
		{
			ALenum err = alGetError();
			if(err != AL_NO_ERROR)
				printf("OpenAL Error: %s (0x%x), @ %d\n", 
					   alGetString(err), err, 873);
		}
	}

	// tell the user what is going on
	if( videodump_required){
#if 0
		CONPRINTF( ConsoleString, 
				   (ConsoleString, "Starting Video Dump to dir %s",
					path));
#endif
	}
	else{
		CONPRINTF( ConsoleString, (ConsoleString, "Video Dump Canceled"));
	}
}

