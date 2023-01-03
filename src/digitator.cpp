#include "digitator.hpp"

#include <fstream>    // files
#include <iostream>   // console

std::string dgt::Digitator::processFile(const std::string& file_path) {
  std::ifstream input_file {file_path};
  std::string file_data {}, aux {};

  // first, lets try to open the file
  if (input_file.is_open()) {
    while (std::getline(input_file, aux)) {
      // add a new line char if you want to keep sentences splitted into various lines if they were so
      file_data += aux + '\n';
    }
    input_file.close();
  } else {
    // something went wrong, probably incorrect path or the file doesn't exists
    std::cerr << "ERROR: Input file can't be opened.\n";
    // return empty string for now
    return std::string {};
  }
  // for now, we have all the data in 1 string
  // but for convenience, lets put each sentence in his own string
  // remember: Each sentence always ends with period
  Sentences sentences {splitIntoSentences(file_data)};

  // convert to digits (if any) each sentence
  for (auto& sentence: sentences) {
    sentence = digitize(sentence);
  }
  // save the string to the member variable
  m_output_string = joinSentences(sentences);
  // requirement nr.5, output to to console
  std::cout << m_output_string << '\n';
  // return all the sentences as string
  return m_output_string;
}

std::string dgt::Digitator::digitize(const std::string& text) const {
  // split the string into words
  Words words {splitIntoWords(text)};
  std::vector<WordInfo> processed_sentence {};

  // index traversal of words
  for (std::size_t i = 0; i < words.size(); ++i) {

    // first, convert it to lower case
    const std::string word_lowercased {toLower(words[i])};

    // is it a period?
    if (word_lowercased == ".") {
      processed_sentence.push_back({WordType::Period, words[i], 0});
      break; // must be last word
    }

    // is an "adder" word?
    if (isAdderWord(word_lowercased)) {
      processed_sentence.push_back({WordType::Adder, words[i], 0});
      continue;
    }

    // is a number?
    // warning: c++17 feature for map traversal in a for each loop
    bool number_found {false};
    for (const auto& [number, digits]: k_nums_to_digits) {
      if (word_lowercased == number) {
        // we find it on the map, so it's a number
        processed_sentence.push_back({WordType::Number, words[i], digits});
        number_found = true;
        break;
      }
    }
    if (number_found) continue;

    // is an exponent?
    bool exponent_found {false};
    for (const auto& [exponent, value]: k_exponents) {
      if (exponent == word_lowercased) {
        // find it!
        processed_sentence.push_back({WordType::Exponent, words[i], value});
        exponent_found = true;
        break;
      }
    }
    if (exponent_found) continue;

    // if it's not one of the other word types, it must be a NotANumber
    processed_sentence.push_back({WordType::NotANumber, words[i], 0});

  } // final loop for words

  // now we have to construct the final sentence with the processed_sentence

  std::vector<WordInfo> final_sentence {};

  for (auto it = processed_sentence.begin(); it != processed_sentence.end(); ++it) {
    // number? numbers always begin with a WordType::Number
    if (it->type == WordType::Number) {
      // create an auxiliary iterator to move forward
      auto aux_it {it};
      // accumulators for the number
      std::size_t acc {0}, super_acc {0};
      // keep looking forward if the number continues
      while(aux_it != processed_sentence.end() && aux_it->type != WordType::NotANumber && aux_it->type != WordType::Period) {

        // it's a number? just add it to the total of the accumulator
        if (aux_it->type == WordType::Number) {
          acc += aux_it->numeric_value;
        }

        // it's an exponent?
        if (aux_it->type == WordType::Exponent) {
          // the tricky part here is the "hundred", because it can appear
          // multiple times, unlike the other exponents.
          // so we multiply it by the accumulator and do not reset anything else
          if (aux_it->numeric_value == 100) {
            acc *= 100;

          } else {
            // accumulator is not "hundred" so we can multiply by the exponent
            // and add the total to the superaccumulator. Reset the accumulator to 0 after.
            super_acc = super_acc + acc * aux_it->numeric_value;
            acc = 0;
          }
        }
        ++aux_it;
      }
      // the number has been processed and it's now ready to be push to the final sentence.
      final_sentence.push_back(WordInfo {WordType::Number, "", super_acc + acc});

      // check if we are at the end of the sentence.
      // if so, break the loop.
      if (aux_it != processed_sentence.end()) {
        // if there's still sentence to check, asign the global iterator
        // where we left after processing the number
        it = aux_it;
        // this is just cosmethics for nice printing
        if (aux_it->text != "." && aux_it->text != "," && aux_it->text != ";" && !isAdderWord(aux_it->text)) {
          final_sentence.push_back(WordInfo {WordType::Adder, " ", 0});
        }
      } else {
        break;
      }
    }
    // else its a normal word, push it
    final_sentence.push_back(*it);
  }
  return joinWords(final_sentence);
}

std::string dgt::Digitator::joinSentences(const Sentences& sentences) const {
  std::string final_string {};
  for (const auto& sentence: sentences) {
    final_string += sentence + '\n';
  }
  return final_string;
}

std::string dgt::Digitator::joinWords(const std::vector<WordInfo>& words) const {
  std::string final_string {};
  for (const auto& word: words) {
    if (word.type == WordType::Number) {
      final_string += std::to_string(word.numeric_value);
    } else {
      final_string += word.text;
    }
  }
  return final_string;
}

dgt::Sentences dgt::Digitator::splitIntoSentences(const std::string& text) const {
  Sentences result {};
  std::string sentence {};
  // for spliting I'll use a simple method:
  // Construct a string with the chars of the text until you find a period.
  // Then push the string to the array and start constructing a new one.
  for (const auto character: text) {
    // this removes the starting newline from the begining of the sentences
    if (character == '\n' && !sentence.size()) continue;

    sentence += character;

    if (character == '.') {
      result.push_back(sentence);
      sentence.clear();
    }
  }
  return result;
}

dgt::Words dgt::Digitator::splitIntoWords(const std::string& text) const {
  Words result {};
  std::string word {};

  for (char character: text) {

    // static_cast: https://en.cppreference.com/w/cpp/string/byte/tolower
    if (isAGoodEnlighNumberChar(std::tolower(static_cast<unsigned char>(character)))) {
      // if true, we have a number or a word, keep stacking
      word += character;
    } else {
      // some rogue character unlike s_english_numbers_chars
      // push what we have until here, clear the word and also push the new weird char
      result.push_back(word);
      word.clear();
      result.push_back(std::string{character});
    }
  }

  return result;
}
