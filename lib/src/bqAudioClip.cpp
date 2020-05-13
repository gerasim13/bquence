#include "bqAudioClip.h"

namespace bq {
ma_uint64 AudioClip::pull_preload(float *dest, ma_uint64 first_pull_frame,
	ma_uint64 num_pull_frames, double orig_factor, double inv_orig_factor)
{
	ma_uint64 num_pulled = 0;

	ma_uint64 preload_orig_last_frame =
		preload.first_frame + static_cast<ma_uint64>(
			static_cast<double>(preload.num_frames) * orig_factor +
			0.5);

	if (preload.frames &&
		first_pull_frame >= preload.first_frame &&
		first_pull_frame < preload_orig_last_frame) {
		ma_uint64 first_actual_pull_frame = static_cast<ma_uint64>(
			static_cast<double>(first_pull_frame -
				preload.first_frame) * inv_orig_factor + 0.5);
		ma_uint64 num_avail_frames = preload.num_frames -
			first_actual_pull_frame;

		if (num_avail_frames > 0) {
			ma_uint64 num_actual_pull_frames =
				num_avail_frames < num_pull_frames ?
				num_avail_frames : num_pull_frames;

			_copy_frames(dest, preload.frames, 0,
				first_actual_pull_frame, num_actual_pull_frames,
				preload.num_channels);

			num_pulled += num_actual_pull_frames;
		}
	}

	return num_pulled;
}

void AudioClip::_copy_frames(float *dest, float *src,
	ma_uint64 dest_first_frame, ma_uint64 src_first_frame,
	ma_uint64 num_frames, ma_uint64 num_channels)
{
	for (ma_uint64 i = 0; i < num_frames; ++i) {
		for (ma_uint64 j = 0; j < num_channels; ++j) {
			ma_uint64 src_idx = (src_first_frame + i) * num_channels
				+ j;
			ma_uint64 dest_idx = (dest_first_frame + i) *
				num_channels + j;

			dest[dest_idx] = src[src_idx];
		}
	}
}
}
