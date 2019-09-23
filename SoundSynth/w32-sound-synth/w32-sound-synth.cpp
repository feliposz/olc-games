// w32-sound-synth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "olcNoiseMaker.h"

using namespace std;

namespace synth {

    enum {
        OSC_SINE,
        OSC_SQUARE,
        OSC_TRIANGLE,
        OSC_SAW_ANALOG,
        OSC_SAW_DIGITAL,
        OSC_NOISE
    };

    struct note {
        int nID;
        double dTimeOn;
        double dTimeOff;
        bool bNotePressed;
        bool bNoteErase;

        note()
        {
            dTimeOn = 0.0;
            dTimeOff = 0.0;
            bNotePressed = false;
            bNoteErase = false;
        }

        void NoteOn(double dTime)
        {
            if (!bNotePressed) {
                dTimeOn = dTime;
                bNotePressed = true;
            }
        }

        void NoteOff(double dTime)
        {
            if (bNotePressed) {
                dTimeOff = dTime;
                bNotePressed = false;
            }
        }
    };

    struct envelope_adsr {
        double dAttackTime;
        double dDecayTime;
        double dReleaseTime;
        double dSustainAmplitude;
        double dStartAmplitude;

        envelope_adsr()
        {
            dAttackTime = 0.1;
            dDecayTime = 0.01;
            dReleaseTime = 0.2;
            dSustainAmplitude = 0.8;
            dStartAmplitude = 1.0;
        }

        double GetAmplitude(double dTime, double dTimeOn, double dTimeOff)
        {
            double dAmplitude = 0.0;

            if (dTimeOn > dTimeOff) {
                double dLifeTime = dTime - dTimeOn;
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
                double dLifeTime = dTime - dTimeOn;
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

                dAmplitude = dReleaseAmplitude - dReleaseAmplitude * ((dTime - dTimeOff) / dReleaseTime);
            }

            if (dAmplitude <= 0.0001) {
                dAmplitude = 0.0;
            }

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

    double GetNoteFreq(int n) {
        double twoRoot12 = pow(2.0, 1.0 / 12.0);
        double dHertz = pow(twoRoot12, n - 12) * 440.0;
        return dHertz;
    }

    struct instrument_base {
        double dVolume = 1.0;
        envelope_adsr envelope;

        virtual double GetSound(double dTime, note &n) = 0;
    };

    struct instrument_harm : public instrument_base {
        instrument_harm()
        {
            dVolume = 0.5;
        }

        virtual double GetSound(double dTime, note &n)
        {
            double dHertz = GetNoteFreq(n.nID);
            double dAmp = envelope.GetAmplitude(dTime, n.dTimeOn, n.dTimeOff);
            if (!n.bNotePressed && dTime > n.dTimeOff && dAmp <= 0.0) {
                n.bNoteErase = true;
            }
            return dVolume * dAmp *
                (1.0 * Oscillator(dHertz, dTime, OSC_SQUARE, 5.0, 0.001)
                    + 0.5 * Oscillator(1.5 * dHertz, dTime, OSC_SQUARE)
                    + 0.25 * Oscillator(2.0 * dHertz, dTime, OSC_SQUARE)
                    + 0.05 * Oscillator(0, dTime, OSC_NOISE));
        }
    };

    struct instrument_bell : public instrument_base {
        instrument_bell()
        {
            dVolume = 0.5;
            envelope.dAttackTime = 0.01;
            envelope.dDecayTime = 1.0;
            envelope.dReleaseTime = 1.0;
            envelope.dSustainAmplitude = 0.0;
        }

        virtual double GetSound(double dTime, note &n)
        {
            double dHertz = GetNoteFreq(n.nID);
            double dAmp = envelope.GetAmplitude(dTime, n.dTimeOn, n.dTimeOff);
            if (!n.bNotePressed && dTime > n.dTimeOff && dAmp <= 0.0) {
                n.bNoteErase = true;
            }
            return dVolume * dAmp *
                (1.0 * Oscillator(2.0 * dHertz, dTime, OSC_SINE, 5.0, 0.001)
                    + 0.5 * Oscillator(3.0 * dHertz, dTime, OSC_SINE)
                    + 0.25 * Oscillator(4.0 * dHertz, dTime, OSC_SINE));
        }
    };

}

vector<synth::note> notes;
synth::instrument_harm instrument;
//synth::instrument_bell instrument;

double SynthWave(double dTime) {
    double dOutput = 0.0;
    for (auto &note : notes) {
        dOutput += instrument.GetSound(dTime, note);
    }
    return dOutput;
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

    while (true) {
        char keys[] = "ZSXCFVGBNJMKQ2WE4R5TY7U8I9OP";

        for (int i = 0; i < 28; i++) {
            if (GetAsyncKeyState(keys[i]) & 0x8000) {
                bool bAlreadyPressed = false;
                for (auto n : notes) {
                    if (i == n.nID && n.bNotePressed) {
                        bAlreadyPressed = true;
                        break;
                    }
                }
                if (!bAlreadyPressed) {
                    synth::note n;
                    n.nID = i;
                    n.NoteOn(sound.GetTime());
                    notes.push_back(n);
                }
            }
            else {
                for (auto &n : notes) {
                    if (i == n.nID && n.bNotePressed) {
                        n.NoteOff(sound.GetTime());
                        break;
                    }
                }
            }
        }

        cout << "\r" << "Notes playing: " << notes.size() << "                   ";

        auto it = notes.begin();
        while (it != notes.end()) {
            //cout << it->nID << ' ' << it->dTimeOn << it->dTimeOff << endl;
            if (it->bNoteErase) {
                it = notes.erase(it);
            }
            else {
                ++it;
            }
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }

    }

    sound.Stop();

    return 0;
}

