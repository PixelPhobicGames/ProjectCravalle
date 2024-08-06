
#pragma once

#include "../External/MpegVideo/Video.hpp"

/* Video Lib Wrapper */

class OTVideo {

  public:
    ray_video_t VideoData;

    bool Load(const char *Path) {
        VideoData = ray_video_open(Path);
    }

    bool Delete() {
        ray_video_destroy(&VideoData);
    }

    bool IsDone() {
        return (VideoData.video_state == RAY_VIDEO_STATE_DONE);
    }

    void Update() {
        ray_video_update(&VideoData, GetFrameTime());
    }
};