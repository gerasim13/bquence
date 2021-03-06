#ifndef BQLIBRARY_H
#define BQLIBRARY_H

#include "bqLibrarySongInfo.h"

#include <miniaudio.h>

#include <vector>

namespace bq {
class Library {
public:
	Library() {}
	~Library() {}

	void set_out_sample_rate(double out_sample_rate);

	unsigned int add_song(const std::string &filename, double sample_rate,
		double bpm);

	// Quick access to any song info property in the library
	// Not prefixed with "get_" for brevity's sake
	double sample_rate(unsigned int song_id) const;
	double bpm(unsigned int song_id) const;
	ma_uint64 beats_to_samples(unsigned int song_id, double beats) const;
	double samples_to_beats(unsigned int song_id, double samples) const;
	ma_uint64 beats_to_out_samples(unsigned int song_id, double beats)
		const;
	double out_samples_to_beats(unsigned int song_id, double out_samples)
		const;
	ma_uint64 samples_self2out(unsigned int song_id, ma_uint64 self_samples)
		const;
	ma_uint64 samples_out2self(unsigned int song_id, ma_uint64 out_samples)
		const;
	// DO NOT CALL THIS IN THE AUDIO THREAD
	// It may involve a string copy, which involves memory allocation
	const std::string filename(unsigned int song_id) const;

	bool is_song_id_valid(unsigned int song_id) const;

private:
	std::vector<LibrarySongInfo> _songs;

	double _out_sample_rate = 0.0;
};
}

#endif
