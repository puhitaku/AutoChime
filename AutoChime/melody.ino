#include "class.h"
using namespace std;


Note::Note(Tone _tone, int _duration) {
  tone = _tone;
  duration = _duration;
}

void Note::SetShortOrNot(bool _is_short) { is_short = _is_short; }
Tone Note::GetTone() { return tone; }
int  Note::GetDuration() { return duration; }
bool Note::GetIsShort() { return is_short; }
NoteL::NoteL(Tone tone, int duration) : Note(tone, duration) { is_short = false; }
NoteS::NoteS(Tone tone, int duration) : Note(tone, duration) { is_short = true; }

Score::Score(vector<Note> _notes) { notes = _notes; }

int Score::GetScoreLength() {
  return notes.size();
}

int Score::GetTimeLength() {
  int sum = 0;
  for(auto n : notes) {
    sum += n.GetDuration();
  }
  return sum;
}

