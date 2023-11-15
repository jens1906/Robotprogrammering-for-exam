float EncoderMultipliers[] = { 1, 0.996 };//The values is found by testing, and the usage can be seen later.
//Graph: https://drive.google.com/file/d/1DbhvCuoILpcMSU08_YMRrD7T6l368Fy1/view?usp=sharing
//Data: https://docs.google.com/spreadsheets/d/13F6HxoBgwMkGPW054eQ8_4Q4qsMWnN_1/edit?usp=drive_link&ouid=101880835896362727209&rtpof=true&sd=true

//This function takes and gets the encoder value from the left encoder and processes it so the value works with the right encoder.
double EncoderL() {
  double countsLeft = abs(encoders.getCountsLeft());
  countsLeft = (countsLeft * EncoderMultipliers[0]);//Multiplies the encoder value with a found value, so the 2 encoders count the 'same' way. This value is found by testing.
  return countsLeft;
}

//This function takes and gets the encoder value from the right encoder and processes is so the value works with the left encoder.
double EncoderR() {
  double countsRight = abs(encoders.getCountsRight());
  countsRight = (countsRight * EncoderMultipliers[1]); //Multiplies the encoder value with a found value, so the 2 encoders count the 'same' way. This value is found by testing.
  return countsRight;
}

//This function updates both encoder values, if wanted.
void Encoders() {
  EncoderL();
  EncoderR();
}

//This function is for resetting both encoders.
void ResetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}