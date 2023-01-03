/**
 * @file digitator.hpp
 * @author Alejandro Castillo Blanco (alexcastilloblanco@gmail.com)
 * @brief
 * @version 0.1
 */

#if !defined(WORD_CONVERTER_SRC_DIGITATOR_HPP_)
#define WORD_CONVERTER_SRC_DIGITATOR_HPP_

#include <map>
#include <string>
#include <vector>

namespace dgt {

using Sentences = std::vector<std::string>;
using Words = Sentences;

/**
 * @brief Class for converting all numbers written
 * in english in the provided text into digits.
 * For later use, you can retrieve the output by calling getOutput().
 */
class Digitator {

 public:

  /**
   * @brief Processes a given file to subsitute all english written words into digitts.
   * File must be plain text.
   * @param file_path The path of the file to process.
   * @return std::string The processed file.
   */
  std::string processFile(const std::string& file_path);

  /**
   * @brief Gets the output string object.
   * You should call processFile() before to get some result.
   * @return The output of processFile() in std::string format.
   */
  auto getOutput() const { return m_output_string; }

  /**
   * @brief Enumeration of the different kinds of words that Digitator knows.
   */
  enum class WordType {
    Period,
    Adder,      // and, space, \n
    NotANumber, // any normal word
    Number,
    Exponent,
  };

  /**
   * @brief Convenience struct that stores usefull information about a word (or character).
   */
  struct WordInfo {
    // The word's type, from WordType enum class.
    WordType type {};
    // The recieved text, unchanged.
    std::string text {};
    // A numeric value whe applicable. If type is WordType::NotANumber, 0.
    std::size_t numeric_value {};
  };

 private:

  /**
   * @brief Checks if a character is used in english written numbers.
   * @tparam T A char, unsigned char.
   * @param character The character to check.
   * @return True if the char is used to write english numbers. False otherwise.
   * @note This is a template b/c my compiler (gcc 12.2.0 x86_64) complains
   * even with the static_cast used before calling this.
   */
  template <typename T>
  bool isAGoodEnlighNumberChar(T character) const {
    return std::string(k_english_numbers_chars).find(static_cast<unsigned char>(character)) != std::string::npos;
  }

  /**
   * @brief Checks if a word is an "adder" word.
   * Lacking a better term, an "adder" word is something that doesn't "break"
   * the construction of a number written in words. Ie: "and".
   * @param text The text to check.
   * @return True if the word is an "adder" word. False otherwise.
   */
  bool isAdderWord(const std::string& text) const {
    for (const auto& word: k_adder_words) {
      if (word == text) return true;
    }
    return false;
  }

  /**
   * @brief Converts a string to lowercase.
   * @param text The string to convert.
   * @return The input string lowercased.
   */
  std::string toLower(const std::string& text) const {
    std::string result {};
    for (const auto character: text) {
      result.push_back(std::tolower(static_cast<unsigned char>(character)));
    }
    return result;
  }

  /**
   * @brief Function in charge of parsing and modifing a sentence so that
   * all numbers written in english are transformed into digits.
   * @param text A text to parse.
   * @return The parsed text.
   */
  std::string digitize(const std::string& text) const;

  /**
   * @brief Joins a sentence (a vector of strings) into a single string.
   * @param sentences The vector of sentences.
   * @return A string with all sentences combined, each separated by a new line
   *  character.
   */
  std::string joinSentences(const Sentences& sentences) const ;

  /**
   * @brief Joins a vector of WordInfo in a single string using it's
   * text or numeric value accordingly.
   * @param words The vector of WordInfo to join.
   * @return A string with all the WordInfos combined that hopefully makes sense.
   */
  std::string joinWords(const std::vector<WordInfo>& words) const ;

  /**
   * @brief Splits a string into sentences. Sentences are separated by a period.
   * @param text The text to split.
   * @return A vector of strings, each a sentence.
   */
  Sentences splitIntoSentences(const std::string& text) const;

  /**
   * @brief Splits a text into words and symbols, each in it's own string.
   * @param text The text to split.
   * @return A vector of strings, each a word or symbol.
   */
  Words splitIntoWords(const std::string& text) const;

  // Where the final output is stored.
  std::string m_output_string {};

  // Valid chars for writting english numbers.
  static constexpr auto k_english_numbers_chars {"abcdefghijklmnopqrstuvwxyz"};

  // Adder words are those who do not break the continuity of an english written number.
  const Words k_adder_words {" ", "-", "and", "\n"};

  // A map of the possible exponents and their values.
  const std::map<std::string, std::size_t> k_exponents {
    {"hundred",  100},
    {"thousand", 1000},
    {"million",  1000000},
    {"millions", 1000000},
    {"billion",  1000000000},
    {"billions", 1000000000},
  };

  // A map of all the needed english numbers and their digits equivalence.
  const std::map<std::string, std::size_t> k_nums_to_digits {
    {"zero", 0},
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9},
    {"ten", 10},
    {"eleven", 11},
    {"twelve", 12},
    {"thirteen", 13},
    {"fourteen", 14},
    {"fifteen", 15},
    {"sixteen", 16},
    {"seventeen", 17},
    {"eighteen", 18},
    {"nineteen", 19},
    {"twenty", 20},
    {"thirty", 30},
    {"forty", 40},
    {"fifty", 50},
    {"sixty", 60},
    {"seventy", 70},
    {"eighty", 80},
    {"ninety", 90},
  };
};

} // namespace dgt

#endif // WORD_CONVERTER_SRC_DIGITATOR_HPP_
