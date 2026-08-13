#include "La2aEmulation.h"
#include <cmath>
#include <algorithm>

void La2aEmulation::prepare(double newSampleRate, int)
{
    sampleRate = newSampleRate;
    reset();

    const double attackTime = 0.010;      // 10 ms
    const double releaseFastTime = 0.060; // 60 ms
    const double releaseSlowTime = 1.5;   // 1.5 s

    attackCoeff = 1.0 - std::exp(-1.0 / (sampleRate * attackTime));
    releaseFastCoeff = 1.0 - std::exp(-1.0 / (sampleRate * releaseFastTime));
    releaseSlowCoeff = 1.0 - std::exp(-1.0 / (sampleRate * releaseSlowTime));
}

void La2aEmulation::reset()
{
    envelope = 0.0;
    sidechainFilterL = 0.0;
    sidechainFilterR = 0.0;
    gainReductionDb = 0.0;
    outputLevelDb = -60.0;
    currentGainReductionDb = 0.0f;
    currentOutputLevelDb = -60.0f;
}

double La2aEmulation::getReleaseCoeff(double env) const
{
    // Doble etapa de liberación: rápida al principio, lenta después
    return (env > 0.1) ? releaseSlowCoeff : releaseFastCoeff;
}

double La2aEmulation::applyEmphasis(double input, double& filterState, float emphasis, double dt)
{
    // Filtro high‑shelf de primer orden para el sidechain
    const double alpha = 1.0 - std::exp(-dt * (1000.0 + 9000.0 * emphasis));
    const double lowpass = filterState + alpha * (input - filterState);
    filterState = lowpass;
    return input + emphasis * 2.0 * (input - lowpass);
}

void La2aEmulation::process(juce::AudioBuffer<float>& buffer, float peakReduction, float outputGain, int mode, float emphasis)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels == 0) return;

    const double dt = 1.0 / sampleRate;
    const double thresholdDb = -40.0 * peakReduction;
    const double ratio = (mode == 1) ? 8.0 : 3.0;
    const double makeupDb = -20.0 + 40.0 * outputGain;
    const double makeupGain = std::pow(10.0, makeupDb / 20.0);
    const double maxReductionDb = 24.0;

    for (int i = 0; i < numSamples; ++i)
    {
        // Sidechain: máximo absoluto de todos los canales (link estéreo)
        double sidechain = 0.0;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sample = buffer.getSample(ch, i);
            double absVal = std::abs((double)sample);
            if (absVal > sidechain) sidechain = absVal;
        }

        double sidechainEmph = applyEmphasis(sidechain, sidechainFilterL, emphasis, dt);

        // Envolvente
        if (sidechainEmph > envelope)
            envelope += attackCoeff * (sidechainEmph - envelope);
        else
            envelope += getReleaseCoeff(envelope) * (sidechainEmph - envelope);

        double envDb = 20.0 * std::log10(envelope + 1e-15);

        if (envDb > thresholdDb)
        {
            double over = envDb - thresholdDb;
            gainReductionDb = over * ratio;
            if (gainReductionDb > maxReductionDb) gainReductionDb = maxReductionDb;
        }
        else
        {
            gainReductionDb = 0.0;
        }

        double gainFactor = std::pow(10.0, -gainReductionDb / 20.0) * makeupGain;

        // Aplicar ganancia y saturación suave
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sample = buffer.getSample(ch, i);
            double processed = sample * gainFactor;
            processed = std::tanh(processed * 1.2) / 1.2; // saturación tipo válvula
            buffer.setSample(ch, i, (float)processed);
        }

        // Nivel de salida pico
        double outAbs = 0.0;
        for (int ch = 0; ch < numChannels; ++ch)
            outAbs = std::max(outAbs, (double)std::abs(buffer.getSample(ch, i)));
        outputLevelDb = 20.0 * std::log10(outAbs + 1e-15);
    }

    currentGainReductionDb = (float)gainReductionDb;
    currentOutputLevelDb = (float)outputLevelDb;
}
