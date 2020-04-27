#ifndef BQAUDIOPLAYHEAD_H
#define BQAUDIOPLAYHEAD_H

#include "bqAudioClip.h"
#include "bqPlayheadChunk.h"
#include "bqConfig.h"

#include <miniaudio.h>
#include <SoundTouchDLL.h>

#include <atomic>

namespace bq {
class IOEngine;

class AudioPlayhead {
public:
	AudioPlayhead() {}
	~AudioPlayhead();

	void set_playback_config(ma_uint32 num_channels, ma_uint32 sample_rate);
	void bind_io_engine(IOEngine *io);

	ma_uint64 pull_stretch(double master_bpm, unsigned int track_idx,
		unsigned int clip_idx, AudioClip &clip, double song_bpm,
		float *dest, ma_uint64 first_frame, ma_uint64 num_frames);

	void receive_chunk(unsigned int track, PlayheadChunk *chunk);

	double get_beat();
	void advance(double beats);
	// Causes free; must only be called within the audio thread
	void jump(double beat);

	// A potential problem would be if this causes all chunks to be popped,
	// and the IOEngine does not re-push the popped chunks, then an audio
	// dropout will occur because necessary chunks will be missing...
	void invalidate_cur_clip_idx(unsigned int track_idx);

	unsigned int get_cur_clip_idx(unsigned int track_idx);
	unsigned int get_cur_song_id(unsigned int track_idx);
	// Causes free; must only be called within the audio thread
	void set_cur_clip_idx(unsigned int track_idx,
		unsigned int cur_clip_idx);
	// Causes free; must only be called within the audio thread
	void set_cur_song_id(unsigned int track_idx, unsigned int cur_song_id);

private:
	void _setup_soundtouch(HANDLE &st);

	ma_uint64 _pull(unsigned int track_idx, AudioClip &clip, float *dest,
		ma_uint64 num_frames);

	static constexpr unsigned int _NUM_TRACKS = WORLD_NUM_TRACKS;
	struct _ChunksList {
		PlayheadChunk *head = nullptr, *tail = nullptr;
		ma_uint64 cur_want_frame = 0;
	};

	void _copy_frames(float *dest, float *src, ma_uint64 dest_first_frame,
		ma_uint64 src_first_frame, ma_uint64 num_frames,
		ma_uint64 num_channels);

	void _delete_chunk(PlayheadChunk *chunk);
	void _pop_chunk(_ChunksList &chunks);
	void _pop_all_chunks(unsigned int track_idx);

	bool _is_track_valid(unsigned int track_idx);

	std::atomic<double> _beat = 0.0;

	// Thank you Mixxx developers for figuring this out!
	// https://github.com/mixxxdj/mixxx/blob/master/src/engine/bufferscalers/enginebufferscalest.cpp#L25
	static constexpr unsigned int _NUM_ST_SRC_FRAMES = 519;
	float *_st_src = nullptr;

	HANDLE _st[_NUM_TRACKS] = { nullptr };

	_ChunksList _cache[_NUM_TRACKS];
	std::atomic<unsigned int> _cur_clip_idxs[_NUM_TRACKS] = { 0 };
	std::atomic<unsigned int> _cur_song_ids[_NUM_TRACKS] = { 0 };
	std::atomic<bool> _cur_clip_idxs_valid[_NUM_TRACKS] = { false };
	std::atomic<bool> _cur_song_ids_valid[_NUM_TRACKS] = { false };
	std::atomic<bool> _needs_seek[_NUM_TRACKS] = { false };

	ma_uint32 _num_channels = 0;
	ma_uint32 _sample_rate = 0;

	IOEngine *_io = nullptr;
};
}

#endif
