#include "htmldecode.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <cassert>

const std::string html_lt = "&lt";
const std::string html_lt__ = "&lt;";
const std::string html_LT = "&LT";
const std::string html_LT__ = "&LT;";

const std::string html_gt = "&gt";
const std::string html_gt__ = "&gt;";
const std::string html_GT = "&GT";
const std::string html_GT__ = "&GT;";

const std::string html_amp = "&amp";
const std::string html_amp__ = "&amp;";
const std::string html_AMP = "&AMP";
const std::string html_AMP__ = "&AMP;";

const std::string html_apos = "&apos";
const std::string html_apos__ = "&apos;";
const std::string html_APOS = "&APOS";
const std::string html_APOS__ = "&APOS;";

const std::string html_quot = "&quot";
const std::string html_quot__ = "&quot;";
const std::string html_QUOT = "&QUOT";
const std::string html_QUOT__ = "&QUOT__";

const char ch_left = '<';
const char ch_right = '>';
const char ch_amp = '&';
const char ch_apos = '\'';
const char ch_quot = '"';

const std::unordered_map<std::string_view, char> html_dec{
    {html_lt, ch_left},
    {html_lt__, ch_left},
    {html_LT, ch_left},
    {html_LT__, ch_left},

    {html_gt, ch_right},
    {html_gt__, ch_right},
    {html_GT, ch_right},
    {html_GT__, ch_right},

    {html_amp, ch_amp},
    {html_amp__, ch_amp},
    {html_AMP, ch_amp},
    {html_AMP__, ch_amp},

    {html_apos, ch_apos},
    {html_apos__, ch_apos},
    {html_APOS, ch_apos},
    {html_APOS__, ch_apos},

    {html_quot, ch_quot},
    {html_quot__, ch_quot},
    {html_QUOT, ch_quot},
    {html_quot__, ch_quot}

};

const std::vector<std::string_view> bucket_with_sequenses{html_amp__, html_AMP__,
                                                          html_APOS__, html_apos__,
                                                          html_GT__, html_gt__,
                                                          html_LT__, html_gt__,
                                                          html_QUOT__, html_quot__,
                                                          html_amp, html_AMP,
                                                          html_APOS, html_apos,
                                                          html_GT, html_gt,
                                                          html_LT, html_gt,
                                                          html_QUOT, html_quot

};

std::string HtmlDecode(std::string_view str)
{

    std::string line = std::string(str);
    std::unordered_map<size_t, std::string_view> queue_to_replace;

    for (std::string_view rm_seq : bucket_with_sequenses)
    {
        size_t pos = 0;
        while ((pos = str.find(rm_seq, pos)) != std::string::npos)
        {
            queue_to_replace.insert({pos, rm_seq});
            pos += rm_seq.length();
        }
    }

    for (auto [pos, word] : queue_to_replace)
    {
        assert(html_dec.count(word) > 0);
        line.replace(pos, word.length(), 1, html_dec.at(word));
    }
    return line;
}
