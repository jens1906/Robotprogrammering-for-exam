float EncoderMultipliers[] = { 1, 0.996 };

double EncoderL() {
  double countsLeft = abs(encoders.getCountsLeft());
  countsLeft = (countsLeft * EncoderMultipliers[0]);
  return countsLeft;
}
double EncoderR() {
  double countsRight = abs(encoders.getCountsRight());
  countsRight = (countsRight * EncoderMultipliers[1]);
  return countsRight;
}
void Encoders() {
  EncoderL();
  EncoderR();
}

void ResetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}