#include "video_decoding_vp.hpp"
#include <stdio.h>
#include <string.h>
#include "utility.h"
#include "logs.h"

namespace oppvs
{
	int VPVideoDecoder::init()
	{
		vpx_codec_iface_t *(*const codec_interface)() = &vpx_codec_vp8_dx;

 		if (vpx_codec_dec_init(&m_codec, codec_interface(), NULL, 0))
 			printf("Init failed\n");
	 	
		return ERRS_DECODING_OK;
	}

	int VPVideoDecoder::init(VideoStreamInfo& info)
	{
		vpx_codec_iface_t *(*const codec_interface)() = &vpx_codec_vp8_dx;
		m_numSources = info.noSources;

		for (int i = 0; i < m_numSources; i++)
		{
			m_controllers[i].source = info.sources[i].source;
			if (vpx_codec_dec_init(&m_controllers[i].codec, codec_interface(), NULL, 0))
			{
 				LOGD("Init failed\n");
 				m_controllers[i].state = false;
 			}
 			else
 			{
 				LOGD("Init codec successfully\n");
 				m_controllers[i].state = true;
 			}
		}
		return ERRS_DECODING_OK;
	}

	int VPVideoDecoder::decode(PixelBuffer& pf, uint32_t length, uint8_t* frame)
	{
		vpx_codec_iter_t iter = NULL;
		vpx_image_t *img = NULL;
		int frame_cnt = 0;
		int error = 0;
		
		vpx_codec_ctx_t *codec = NULL;
		for (int i = 0; i < m_numSources; i++)
		{
			if (m_controllers[i].source == pf.source && m_controllers[i].state)
			{
				codec = &m_controllers[i].codec;
			}
		}

		if (!codec)
		{
			printf("Cannot find codec to decode frame\n");
			return -1;
		}

		if (vpx_codec_decode(codec, frame, (unsigned int)length, NULL, 0) != VPX_CODEC_OK)
		{
    		printf("Failed to decode frame.\n");
    		return -1;
    	}
	    while ((img = vpx_codec_get_frame(codec, &iter)) != NULL) {
	    	int corrupted = 0;
	    	vpx_codec_control(codec, VP8D_GET_FRAME_CORRUPTED, &corrupted);
			if (corrupted) {
				LOGD("corrupted\n");
				error = 0;
				//vpx_codec_control(codec, VP8D_GET_LAST_REF_USED, &corrupted);
				//printf("ref use: %d\n", corrupted);
				//return error;
			}

			//printf("%d %d %d %d\n", img->w, img->d_w, img->stride[1], img->stride[2]);
	      	if (updateImage(pf, img) < 0)
	      	{
	      		//return -1;
	      	}
	     	++frame_cnt;
	     	//printf("Frame count: %u len: %u\n", frame_cnt, length);
	    }
	    return error;
	}

	int VPVideoDecoder::updateImage(PixelBuffer& pf, vpx_image_t* img)
	{

		pf.plane[0] = new uint8[pf.stride[0]*pf.height[0]];
		int result = 0;
		
#ifndef ANDROID
		pf.format = PF_BGRA32;
		result = libyuv::I420ToARGB(img->planes[0], img->stride[0],
	               img->planes[1], img->stride[1],
	               img->planes[2], img->stride[2],
	               pf.plane[0], pf.stride[0],
	               pf.width[0], pf.height[0]);
#else
		switch (pf.format)
		{
			case PF_BGRA32:
				result = libyuv::I420ToABGR(img->planes[0], img->stride[0],
	               img->planes[1], img->stride[1],
	               img->planes[2], img->stride[2],
	               pf.plane[0], pf.stride[0],
	               pf.width[0], pf.height[0]);
				break;
			case PF_RGBA32:
				result = libyuv::I420ToARGB(img->planes[0], img->stride[0],
	               img->planes[1], img->stride[1],
	               img->planes[2], img->stride[2],
	               pf.plane[0], pf.stride[0],
	               pf.width[0], pf.height[0]);
				break;
			default:
				result = 0;
				break;
		}
		
#endif
		pf.nbytes = pf.stride[0] * pf.height[0];
		return result;
	}

	int VPVideoDecoder::release()
	{
 		for (int i = 0; i < m_numSources; i++)
		{
			vpx_codec_destroy(&m_controllers[i].codec);
		}

 		if (vpx_codec_destroy(&m_codec))
    		printf("Failed to destroy codec.\n");

		return 0;
	}
}