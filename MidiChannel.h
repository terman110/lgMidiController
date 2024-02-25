int ReadMidiChannel(int pinCh1, int pinCh2, int pinCh3, int pinCh4)
{
    pinMode(pinCh1, INPUT_PULLUP);
    pinMode(pinCh2, INPUT_PULLUP);
    pinMode(pinCh3, INPUT_PULLUP);
    pinMode(pinCh4, INPUT_PULLUP);
    
    int valCh1 = !digitalRead(pinCh1);
    int valCh2 = !digitalRead(pinCh2);
    int valCh3 = !digitalRead(pinCh3);
    int valCh4 = !digitalRead(pinCh4);

    return 1 + (valCh1 << 0 | valCh2 << 1 | valCh3 << 2 | valCh4 << 3);
}
