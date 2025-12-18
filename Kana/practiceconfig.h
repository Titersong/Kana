#ifndef PRACTICECONFIG_H
#define PRACTICECONFIG_H

struct PracticeConfig
{
    enum class Mode   { KanaToRomaji, RomajiToKana, Mixed };
    enum class Script { Hiragana, Katakana, Both };
    enum class Source { All, Mastered};

    Mode   mode   = Mode::Mixed;
    Script script = Script::Both;
    Source source = Source::All;
    int questionLimit = -1; // -1 = infinite
};

#endif // PRACTICECONFIG_H
