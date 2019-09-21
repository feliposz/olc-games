// w32-sound-synth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "olcNoiseMaker.h"

using namespace std;

enum {
    OSC_SINE,
    OSC_SQUARE,
    OSC_TRIANGLE,
    OSC_SAW_ANALOG,
    OSC_SAW_DIGITAL,
    OSC_NOISE
};

struct SEnvelopeADSR {
    double dAttackTime;
    double dDecayTime;
    double dReleaseTime;
    double dSustainAmplitude;
    double dStartAmplitude;
    double dTriggerOnTime;
    double dTriggerOffTime;
    bool bNoteOn;

    SEnvelopeADSR()
    {
        dAttackTime = 0.1;
        dDecayTime = 0.01;
        dReleaseTime = 0.2;
        dSustainAmplitude = 0.8;
        dStartAmplitude = 1.0;
        dTriggerOnTime = 0.0;
        dTriggerOffTime = 0.0;
        bNoteOn = false;
    }

    void NoteOn(double dTime)
    {
        if (!bNoteOn) {
            dTriggerOnTime = dTime;
            bNoteOn = true;
        }
    }

    void NoteOff(double dTime)
    {
        if (bNoteOn) {
            dTriggerOffTime = dTime;
            bNoteOn = false;
        }
    }

    double GetAmplitude(double dTime)
    {
        double dAmplitude = 0.0;

        if (bNoteOn) {
            double dLifeTime = dTime - dTriggerOnTime;
            if (dLifeTime <= dAttackTime) {
                dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
            }
            else if (dLifeTime <= (dAttackTime + dDecayTime)) {
                dAmplitude = ((dLifeTime - dAttackTime) / dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;
            }
            else {
                dAmplitude = dSustainAmplitude;
            }
        }
        else {
            double dLifeTime = dTime - dTriggerOnTime;
            double dReleaseAmplitude = 0.0;

            if (dLifeTime <= dAttackTime) {
                dReleaseAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
            }
            else if (dLifeTime <= (dAttackTime + dDecayTime)) {
                dReleaseAmplitude = ((dLifeTime - dAttackTime) / dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;
            }
            else {
                dReleaseAmplitude = dSustainAmplitude;
            }

            dAmplitude = dReleaseAmplitude - dReleaseAmplitude * ((dTime - dTriggerOffTime) / dReleaseTime);
        }

        if (dAmplitude <= 0.0001) {
            dAmplitude = 0.0;
        }

        //cout << dTime << " -> " << dAmplitude << endl;

        return dAmplitude;
    }
};

double ToRad(double angle) {
    return angle * 2.0 * PI;
}

double Oscillator(double dHertz, double dTime, int nType, double dLFOHertz = 0.0, double dLFOAmplitude = 0.0)
{
    double dValue = 0.0;
    double dFreq = ToRad(dHertz) * dTime + (dLFOAmplitude * dHertz * sin(ToRad(dLFOHertz) * dTime));

    switch (nType) {

    case OSC_SINE:
        dValue = sin(dFreq);
        dValue *= 1.0;
        break;

    case OSC_SQUARE:
        dValue = sin(dFreq) > 0.0 ? 0.5 : -0.5;
        break;

    case OSC_TRIANGLE:
        dValue = asin(sin(dFreq)) * 2.0 / PI;
        break;

    case OSC_SAW_ANALOG:
        for (int i = 1; i < 40; i++) {
            dValue += sin(dFreq * i) * 1.0 / i;
        }
        dValue *= 1.0 / PI;
        break;

    case OSC_SAW_DIGITAL:
        // TODO: Make LFO work with this wave type also
        dValue = (1.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));
        break;

    case OSC_NOISE:
        dValue = 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
        break;

    default:
        dValue = 0;
        break;
    }

    return dValue;
}

atomic<double> dHertz = 0.0;

struct SInstrument {
    double dVolume = 1.0;
    SEnvelopeADSR envelope;

    virtual double GetSound(double dHertz, double dTime) = 0;
};

struct Harmonica : public SInstrument {
    Harmonica()
    {
        dVolume = 0.5;
    }

    virtual double GetSound(double dHertz, double dTime)
    {
        return dVolume * envelope.GetAmplitude(dTime) * (1.0 * Oscillator(dHertz, dTime, OSC_SQUARE, 5.0, 0.001)
            + 0.5 * Oscillator(1.5 * dHertz, dTime, OSC_SQUARE)
            + 0.25 * Oscillator(2.0 * dHertz, dTime, OSC_SQUARE)
            + 0.05 * Oscillator(0, dTime, OSC_NOISE));
    }
};


struct Bell : public SInstrument {
    Bell()
    {
        dVolume = 0.5;
        envelope.dAttackTime = 0.01;
        envelope.dDecayTime = 1.0;
        envelope.dReleaseTime = 1.0;
        envelope.dSustainAmplitude = 0.0;
    }

    virtual double GetSound(double dHertz, double dTime)
    {
        return dVolume * envelope.GetAmplitude(dTime) * (1.0 * Oscillator(2.0 * dHertz, dTime, OSC_SINE, 5.0, 0.001)
            + 0.5 * Oscillator(3.0 * dHertz, dTime, OSC_SINE)
            + 0.25 * Oscillator(4.0 * dHertz, dTime, OSC_SINE));
    }
};

Harmonica instrument;
//Bell instrument;

double SynthWave(double dTime) {
    return instrument.GetSound(dHertz, dTime);
}

int main()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Hide cursor
    CONSOLE_CURSOR_INFO CursorInfo;
    CursorInfo.bVisible = false;
    CursorInfo.dwSize = 1;
    SetConsoleCursorInfo(hStdOut, &CursorInfo);

    vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

    for (auto d : devices) {
        wcout << L"Detected: " << d << endl;
    }

    int nSampleRate = 44100;
    int nChannels = 1;
    int nBlocks = 8;
    int nBlockSamples = 512;

    wcout << L"Using device: " << devices[0] << endl;

    olcNoiseMaker<short> sound(devices[0], nSampleRate, nChannels, nBlocks, nBlockSamples);

    sound.SetUserFunction(SynthWave);

    cout << "  _____________________________________________________________________" << endl;
    cout << "  |  | |  |  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |" << endl;
    cout << "  |  | |  |  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |" << endl;
    cout << "  |  |S|  |  |F| |G|  |  |J| |K| |2|  |  |4| |5|  |  |7| |8| |9|  |   |" << endl;
    cout << "  |  | |  |  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |" << endl;
    cout << "  |  |_|  |  |_| |_|  |  |_| |_| |_|  |  |_| |_|  |  |_| |_| |_|  |   |" << endl;
    cout << "  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |" << endl;
    cout << "  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |" << endl;
    cout << "  | Z | X | C | V | B | N | M | Q | W | E | R | T | Y | U | I | O | P |" << endl;
    cout << "  |___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|" << endl << endl;
    cout << "<ESC> Quit" << endl << endl;

    double twoRoot12 = pow(2.0, 1.0 / 12.0);

    while (true) {
        char keys[] = "ZSXCFVGBNJMKQ2WE4R5TY7U8I9OP";

        bool bPressed = false;
        for (int i = 0; i < 28; i++) {
            if (GetAsyncKeyState(keys[i]) & 0x8000) {
                instrument.envelope.NoteOn(sound.GetTime());
                dHertz = pow(twoRoot12, i - 12) * 440.0;
                cout << "\r" << "Pressed: " << keys[i] << " Playing: " << dHertz << "hz                   ";
                bPressed = true;
            }
        }

        if (!bPressed) {
            cout << "\r" << "No key pressed.                   ";
            instrument.envelope.NoteOff(sound.GetTime());
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        
    }

    sound.Stop();

    return 0;
}

