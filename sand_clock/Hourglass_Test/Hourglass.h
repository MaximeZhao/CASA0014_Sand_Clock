#pragma once
#include <stdint.h>
#include <stddef.h>

// Independent of Arduino hardware: 0 = contact closed, 1 = contact open.
class Hourglass {
 public:
  explicit Hourglass(bool closedMeansAUp = true,
                     uint32_t debounceMs = 200,
                     float fullSeconds = 18.0f)
      : closedMeansAUp_(closedMeansAUp), debounceMs_(debounceMs),
        fullSeconds_(fullSeconds > 0.0f ? fullSeconds : 18.0f) {}

  void begin(uint32_t now, int raw) {
    candidate_ = raw;
    changedAt_ = previousTime_ = now;
    begun_ = true;
    ready_ = false;
    sandA_ = 0.5f;
  }

  void update(uint32_t now, int raw) {
    if (!begun_) { begin(now, raw); return; }
    uint32_t elapsed = now - previousTime_;
    previousTime_ = now;

    if (raw != candidate_) {
      candidate_ = raw;
      changedAt_ = now;
    }
    const bool settled = (uint32_t)(now - changedAt_) >= debounceMs_;

    if (!ready_) {
      if (!settled) return;
      stable_ = candidate_;
      ready_ = true;
      // On first stable reading, put all light-sand in the upper reservoir.
      sandA_ = aUp() ? 1.0f : 0.0f;
      return;
    }

    // During a physical flip, pause rather than interpreting switch chatter.
    if (!settled) return;
    if (stable_ != candidate_) {
      stable_ = candidate_;
      return;  // Preserve the amount; do not reset on subsequent flips.
    }

    // Do not jump ahead after a blocking connection attempt.
    if (elapsed > 250) return;
    float movement = (float)elapsed / (fullSeconds_ * 1000.0f);
    sandA_ += aUp() ? -movement : movement;
    if (sandA_ < 0.0f) sandA_ = 0.0f;
    if (sandA_ > 1.0f) sandA_ = 1.0f;
  }

  bool ready() const { return ready_; }
  bool aUp() const {
    return closedMeansAUp_ ? stable_ == 0 : stable_ != 0;
  }
  float amountA() const { return sandA_; }
  float amountB() const { return 1.0f - sandA_; }

  // Prototype layout: A = logical pixels 0..35, B = 36..71.
  // This is NOT a verified physical map of the Vespera installation.
  bool render(uint8_t* output, size_t length) const {
    if (length != 216) return false;
    for (size_t i = 0; i < length; ++i) output[i] = 0;
    if (!ready_) return true;
    const float amounts[2] = {amountA(), amountB()};
    const uint8_t colours[2][3] = {{70, 22, 0}, {0, 22, 70}};
    for (int group = 0; group < 2; ++group) {
      float lit = amounts[group] * 36.0f;
      for (int i = 0; i < 36; ++i) {
        float fraction = lit - (float)i;
        if (fraction < 0.0f) fraction = 0.0f;
        if (fraction > 1.0f) fraction = 1.0f;
        int pixel = group == 0 ? i : 71 - i;
        for (int channel = 0; channel < 3; ++channel) {
          output[pixel * 3 + channel] =
              (uint8_t)(colours[group][channel] * fraction + 0.5f);
        }
      }
    }
    return true;
  }

 private:
  bool closedMeansAUp_;
  uint32_t debounceMs_;
  float fullSeconds_;
  bool begun_ = false;
  bool ready_ = false;
  int candidate_ = 1;
  int stable_ = 1;
  uint32_t changedAt_ = 0;
  uint32_t previousTime_ = 0;
  float sandA_ = 0.5f;
};

