/******************************************************************************
 *
 * @file
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *   Name:       test_utils.hpp
 *   Project:    RED
 *
 *   Description: Test utilities and base classes for RED tests
 *
 *   Created:    3/20/2025
 *   Author(s):  Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <map>

/* Test categories */
enum class TestCategory
{
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    REGRESSION,
    SMOKE
};

/* Convert TestCategory to string */
inline std::string TestCategoryToString(TestCategory category)
{
    static const std::map<TestCategory, std::string> category_map = {
        {TestCategory::UNIT, "UNIT"},
        {TestCategory::INTEGRATION, "INTEGRATION"},
        {TestCategory::PERFORMANCE, "PERFORMANCE"},
        {TestCategory::REGRESSION, "REGRESSION"},
        {TestCategory::SMOKE, "SMOKE"}};
    return category_map.at(category);
}

/* Custom test listener for better reporting */
class TestEventListener : public testing::EmptyTestEventListener
{
private:
    std::chrono::steady_clock::time_point suite_start_;
    std::chrono::steady_clock::time_point test_start_;
    std::vector<std::string>              failed_tests_;
    std::ofstream                         xml_output_;
    std::ofstream                         json_output_;
    int                                   total_tests_        = 0;
    int                                   failed_tests_count_ = 0;
    std::string                           current_suite_;
    std::string                           escape_xml(const std::string &input)
    {
        std::string output = input;
        size_t      pos    = 0;
        while ((pos = output.find_first_of("\"'&<>", pos)) != std::string::npos)
        {
            std::string replacement;
            switch (output[pos])
            {
            case '\"':
                replacement = "&quot;";
                break;
            case '\'':
                replacement = "&apos;";
                break;
            case '&':
                replacement = "&amp;";
                break;
            case '<':
                replacement = "&lt;";
                break;
            case '>':
                replacement = "&gt;";
                break;
            default:
                ++pos;
                continue;
            }
            output.replace(pos, 1, replacement);
            pos += replacement.length();
        }
        return output;
    }

public:
    explicit TestEventListener(const std::string &xml_path  = "test_results.xml",
                               const std::string &json_path = "test_results.json")
    : xml_output_(xml_path),
      json_output_(json_path)
    {
        /* Initialize XML output */
        xml_output_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                    << "<testsuites>\n";

        /* Initialize JSON output */
        json_output_ << "{\n"
                     << "  \"testsuites\": [\n";
    }

    ~TestEventListener()
    {
        /* Close XML output */
        xml_output_ << "</testsuites>\n";
        xml_output_.close();

        /* Close JSON output */
        json_output_ << "  ]\n}\n";
        json_output_.close();
    }

    void OnTestProgramStart(const testing::UnitTest & /*unit_test*/) override
    {
        std::cout << "\n=== Test Program Start ===\n\n";
    }

    void OnTestSuiteStart(const testing::TestSuite &test_suite) override
    {
        current_suite_ = test_suite.name();
        suite_start_   = std::chrono::steady_clock::now();
        std::cout << "\nTest Suite: " << current_suite_ << "\n"
                  << std::string(60, '-') << "\n";

        /* Start test suite in JSON */
        json_output_ << "    {\n"
                     << "      \"name\": \"" << current_suite_ << "\",\n"
                     << "      \"tests\": [\n";
    }

    void OnTestStart(const testing::TestInfo &test_info) override
    {
        test_start_ = std::chrono::steady_clock::now();
        total_tests_++;
        std::cout << "[ RUN     ] " << test_info.test_suite_name() << "."
                  << test_info.name() << "\n";
    }

    void OnTestPartResult(const testing::TestPartResult &result) override
    {
        if (result.failed())
        {
            std::cout << "\nFailure in " << result.file_name() << ":"
                      << result.line_number() << "\n"
                      << result.summary() << "\n";
        }
    }

    void OnTestEnd(const testing::TestInfo &test_info) override
    {
        auto end      = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - test_start_);
        bool passed = test_info.result()->Passed();

        /* Console output */
        if (passed)
        {
            std::cout << "[     OK ] ";
        }
        else
        {
            std::cout << "[ FAILED ] ";
            failed_tests_.push_back(std::string(test_info.test_suite_name()) + "." +
                                    test_info.name());
            failed_tests_count_++;
        }
        std::cout << test_info.test_suite_name() << "." << test_info.name() << " ("
                  << duration.count() << " ms)\n";

        /* XML output */
        xml_output_ << "  <testcase name=\"" << escape_xml(test_info.name())
                    << "\" classname=\"" << escape_xml(test_info.test_suite_name())
                    << "\" time=\"" << duration.count() / 1000.0 << "\"";

        if (!passed)
        {
            xml_output_
                << ">\n    <failure message=\"Test failed\"></failure>\n  </testcase>\n";
        }
        else
        {
            xml_output_ << "/>\n";
        }

        /* JSON output */
        json_output_ << "        {\n"
                     << "          \"name\": \"" << test_info.name() << "\",\n"
                     << "          \"status\": \"" << (passed ? "PASSED" : "FAILED")
                     << "\",\n"
                     << "          \"time\": " << duration.count() << ",\n"
                     << "          \"category\": \""
                     << (test_info.value_param() ? test_info.value_param() : "UNIT")
                     << "\"\n"
                     << "        }";

        /* Add comma if not the last test in the suite */
        if (test_info.test_suite_name() == current_suite_)
        {
            json_output_ << ",";
        }
        json_output_ << "\n";
    }

    void OnTestSuiteEnd(const testing::TestSuite &test_suite) override
    {
        auto end      = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - suite_start_);
        std::cout << std::string(60, '-') << "\n"
                  << "Test suite '" << test_suite.name() << "' completed in "
                  << duration.count() << " ms\n\n";

        /* Close the test suite in JSON */
        json_output_ << "      ]\n    },\n";
    }

    void OnTestProgramEnd(const testing::UnitTest &unit_test) override
    {
        std::cout << "\n=== Test Program Summary ===\n"
                  << "Total Tests: " << total_tests_ << "\n"
                  << "Passed: " << (total_tests_ - failed_tests_count_) << "\n"
                  << "Failed: " << failed_tests_count_ << "\n";

        if (!failed_tests_.empty())
        {
            std::cout << "\nFailed Tests:\n";
            for (const auto &test : failed_tests_)
            {
                std::cout << "  " << test << "\n";
            }
        }

        std::cout << "\nTest Program " << (unit_test.Passed() ? "PASSED" : "FAILED")
                  << "\n"
                  << "Test results written to:\n"
                  << "  XML: test_results.xml\n"
                  << "  JSON: test_results.json\n";
    }
};

/* Base test fixture with common functionality */
class TestBase : public testing::Test
{
protected:
    void SetUp() override
    {
        test_start_time_ = std::chrono::steady_clock::now();
    }

    void TearDown() override
    {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - test_start_time_);
        std::cout << "Test took " << duration.count() << " ms\n";
    }

    /* Helper to format time durations */
    static std::string FormatDuration(std::chrono::milliseconds duration)
    {
        auto ms  = duration.count();
        auto sec = ms / 1000;
        ms %= 1000;
        return std::to_string(sec) + "." +
               std::string(3 - std::to_string(ms).length(), '0') + std::to_string(ms) +
               " seconds";
    }

    /* Helper to check if a test should be skipped */
    static void SkipTest(const std::string &reason)
    {
        std::cout << "[ SKIPPED ] " << reason << "\n";
        GTEST_SKIP();
    }

    /* Set test category */
    static void SetTestCategory(TestCategory category)
    {
        ::testing::Test::RecordProperty("category", TestCategoryToString(category));
    }

private:
    std::chrono::steady_clock::time_point test_start_time_;
};

#endif /* TEST_UTILS_HPP */
