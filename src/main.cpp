#include "entropy.hpp"
#include "large_wl.h"
#include "small_wl.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

int main() {

  std::vector<std::pair<double, std::string>> entropies;
  entropies.reserve(wordlist.size());

  for (const auto &word0 : wordlist) {
    std::unordered_map<std::bitset<10>, double> feedback;
    for (const auto &word1 : small_word_list) {
      ++feedback[feedback_word(word0, word1)];
    }
    entropies.emplace_back(entropy(feedback), word0);
  }

  std::sort(entropies.begin(), entropies.end(), std::less{});
  for (const auto &[score, word] : entropies) {
    std::cout << score << " " << word << '\n';
  }

  std::vector<std::string> known_substrings{};
  std::vector<char> include_letters{};
  std::vector<char> exclude_letters{};
  std::vector<char> letters_go_here(5, '0');
  std::vector<int> pos_positions{};
  std::vector<char> letters_dont_go_here(5, '0');
  std::vector<int> neg_positions{};
  std::vector<std::string> possible_words{};
  possible_words.reserve(small_word_list.size());

  // begin feedback loop -------------------------------------------------------
  for (int i = 0; i < 5; ++i) {

    std::stringstream ss{};
    std::string input{};
    std::string substring{};
    char ch{};

    std::cout << "Exclude letters: ";
    std::getline(std::cin, input);
    ss.str(input);
    ss.clear();
    while (ss >> ch) {
      exclude_letters.push_back(ch);
    }
    std::sort(exclude_letters.begin(), exclude_letters.end());
    exclude_letters.erase(
        std::unique(exclude_letters.begin(), exclude_letters.end()),
        exclude_letters.end());
    for (const auto c : exclude_letters) {
      std::cout << c << " ";
    }
    std::cout << '\n';
    if (i == 0) {
      for (const auto &word : small_word_list) {
        bool found{false};
        for (const auto letter : exclude_letters) {
          if (word.find(letter) != std::string::npos) {
            found = true;
            break;
          }
        }
        if (!found) {
          possible_words.push_back(word);
        }
      }
    } else {
      for (auto word = possible_words.begin(); word != possible_words.end();) {
        bool found{false};
        for (const auto letter : exclude_letters) {
          if (word->find(letter) != std::string::npos) {
            found = true;
            break;
          }
        }
        if (found) {
          word = possible_words.erase(word);
        } else {
          ++word;
        }
      }
    }

    std::cout << "Include letters: ";
    std::getline(std::cin, input);
    ss.str(input);
    ss.clear();
    while (ss >> ch) {
      include_letters.push_back(ch);
    }
    std::sort(include_letters.begin(), include_letters.end());
    include_letters.erase(
        std::unique(include_letters.begin(), include_letters.end()),
        include_letters.end());
    for (const auto c : include_letters) {
      std::cout << c << " ";
    }
    std::cout << '\n';

    for (auto word = possible_words.begin(); word != possible_words.end();) {
      bool found{true};
      for (const auto letter : include_letters) {
        if (word->find(letter) == std::string::npos) {
          found = false;
          break;
        }
      }
      if (!found) {
        word = possible_words.erase(word);
      } else {
        ++word;
      }
    }

    std::cout << "Letters in known positions: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
      for (auto j = 0; j < 5; ++j) {
        if (input[j] != '0') {
          pos_positions.push_back(j);
        }
        letters_go_here[j] = input[j];
      }
    }
    for (const auto chr : letters_go_here) {
      std::cout << chr;
    }
    std::cout << '\n';
    std::sort(pos_positions.begin(), pos_positions.end());
    pos_positions.erase(std::unique(pos_positions.begin(), pos_positions.end()),
                        pos_positions.end());
    for (auto word = possible_words.begin(); word != possible_words.end();) {
      bool found{true};
      for (const auto index : pos_positions) {
        if (word->at(index) != letters_go_here[index]) {
          found = false;
          break;
        }
      }
      if (!found) {
        word = possible_words.erase(word);
      } else {
        ++word;
      }
    }

    std::cout << "Letters must not be here: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
      for (auto j = 0; j < 5; ++j) {
        if (input[j] != '0') {
          neg_positions.push_back(j);
        }
        letters_dont_go_here[j] = input[j];
      }
    }
    std::sort(neg_positions.begin(), neg_positions.end());
    neg_positions.erase(std::unique(neg_positions.begin(), neg_positions.end()),
                        neg_positions.end());
    for (const auto chr : letters_dont_go_here) {
      std::cout << chr;
    }
    std::cout << '\n';
    for (auto word = possible_words.begin(); word != possible_words.end();) {
      bool found{false};
      for (const auto index : neg_positions) {
        if (word->at(index) == letters_dont_go_here[index]) {
          found = true;
          break;
        }
      }
      if (found) {
        word = possible_words.erase(word);
      } else {
        ++word;
      }
    }

    if (possible_words.size() == 1) {
      std::cout << possible_words.front() << '\n';
    } else {

      entropies.clear();
      entropies.reserve(possible_words.size());

      auto select_list = [&]() -> const std::vector<std::string> * {
        if (i == 0 && possible_words.size() > 5) {
          return &wordlist;
        }
        if (possible_words.size() <= 5) {
          return &possible_words;
        }
        return &small_word_list;
      };

      for (const auto &word0 : *select_list()) {
        std::unordered_map<std::bitset<10>, double> feedback;
        for (const auto &word1 : possible_words) {
          ++feedback[feedback_word(word0, word1)];
        }
        entropies.emplace_back(entropy(feedback), word0);
      }

      std::sort(entropies.begin(), entropies.end(), std::less{});
      for (const auto &[score, word] : entropies) {
        std::cout << std::setprecision(3) << score << " " << word << '\n';
      }

      std::cout << possible_words.size() << '\n';
    }
  }
  return 0;
}
