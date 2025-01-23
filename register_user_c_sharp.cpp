//Dependencies:
//
//Selenium WebDriver: Install via NuGet (Install-Package Selenium.WebDriver).
//
//CsvHelper: Install via NuGet (Install-Package CsvHelper).

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CsvHelper;
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using OpenQA.Selenium.Support.UI;

namespace RegistrationAutomation
{
    class Program
    {
        private const string BaseUrl = "https://account.ocas.ca/en-CA/Account/Register";

        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: RegistrationAutomation --input <input_csv_path> --output <output_csv_path>");
                return;
            }

            string inputCsvPath = args[1];
            string outputCsvPath = args[3];

            ProcessCsv(inputCsvPath, outputCsvPath);
            Console.WriteLine($"Results have been written to {outputCsvPath}");
        }

        static string GeneratePassword(int length = 12)
        {
            const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+";
            var random = new Random();
            return new string(Enumerable.Repeat(chars, length).Select(s => s[random.Next(s.Length)]).ToArray());
        }

        static Dictionary<string, string> Execute(Dictionary<string, string> inputData)
        {
            string password = GeneratePassword();
            inputData["Password"] = password;
            inputData["Confirm Password"] = password;

            IWebDriver driver = new ChromeDriver(); // Ensure chromedriver is in your PATH
            driver.Navigate().GoToUrl(BaseUrl);

            try
            {
                WebDriverWait wait = new WebDriverWait(driver, TimeSpan.FromSeconds(10));
                wait.Until(d => d.FindElement(By.Id("firstName")));

                driver.FindElement(By.Id("firstName")).SendKeys(inputData["Legal First / Given Name"]);
                driver.FindElement(By.Id("lastName")).SendKeys(inputData["Legal Last / Family Name"]);
                driver.FindElement(By.Id("email")).SendKeys(inputData["Email Address"]);
                driver.FindElement(By.Id("password")).SendKeys(inputData["Password"]);
                driver.FindElement(By.Id("confirmPassword")).SendKeys(inputData["Confirm Password"]);

                var securityQuestions = new List<Dictionary<string, string>>
                {
                    new Dictionary<string, string> { { "Question", inputData["Security Question 1"] }, { "Answer", inputData["Security Answer 1"] } },
                    new Dictionary<string, string> { { "Question", inputData["Security Question 2"] }, { "Answer", inputData["Security Answer 2"] } },
                    new Dictionary<string, string> { { "Question", inputData["Security Question 3"] }, { "Answer", inputData["Security Answer 3"] } }
                };

                for (int i = 0; i < 3; i++)
                {
                    string questionId = $"securityQuestion{i}Id";
                    string answerId = $"securityQuestion{i}Answer";

                    var questionDropdown = new SelectElement(driver.FindElement(By.Id(questionId)));
                    var options = questionDropdown.Options.Select(o => o.Text).ToList();

                    if (i < securityQuestions.Count)
                    {
                        string providedQuestion = securityQuestions[i]["Question"];
                        string providedAnswer = securityQuestions[i]["Answer"];

                        if (options.Contains(providedQuestion))
                        {
                            questionDropdown.SelectByText(providedQuestion);
                            driver.FindElement(By.Id(answerId)).SendKeys(providedAnswer);
                        }
                        else
                        {
                            questionDropdown.SelectByIndex(1);
                            driver.FindElement(By.Id(answerId)).SendKeys(providedAnswer);
                        }
                    }
                    else
                    {
                        questionDropdown.SelectByIndex(1);
                        string randomAnswer = new string(Enumerable.Repeat("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 10)
                            .Select(s => s[new Random().Next(s.Length)]).ToArray());
                        driver.FindElement(By.Id(answerId)).SendKeys(randomAnswer);
                    }
                }

                driver.FindElement(By.CssSelector("label[for='privacyStatement']")).Click();
                driver.FindElement(By.CssSelector("button[type='submit']")).Click();

                inputData["Status"] = "Success";
                return inputData;
            }
            catch (Exception e)
            {
                inputData["Status"] = "Failed";
                return inputData;
            }
            finally
            {
                driver.Quit();
            }
        }

        static void ProcessCsv(string inputCsvPath, string outputCsvPath)
        {
            using (var reader = new StreamReader(inputCsvPath))
            using (var csv = new CsvReader(reader, System.Globalization.CultureInfo.InvariantCulture))
            {
                var records = csv.GetRecords<dynamic>().ToList();
                var results = new List<Dictionary<string, string>>();

                foreach (var record in records)
                {
                    var inputData = new Dictionary<string, string>
                    {
                        { "Legal First / Given Name", record.LegalFirstGivenName },
                        { "Legal Last / Family Name", record.LegalLastFamilyName },
                        { "Email Address", record.EmailAddress },
                        { "Security Question 1", record.SecurityQuestion1 },
                        { "Security Answer 1", record.SecurityAnswer1 },
                        { "Security Question 2", record.SecurityQuestion2 },
                        { "Security Answer 2", record.SecurityAnswer2 },
                        { "Security Question 3", record.SecurityQuestion3 },
                        { "Security Answer 3", record.SecurityAnswer3 }
                    };

                    var result = Execute(inputData);
                    results.Add(result);
                }

                using (var writer = new StreamWriter(outputCsvPath))
                using (var csvWriter = new CsvWriter(writer, System.Globalization.CultureInfo.InvariantCulture))
                {
                    csvWriter.WriteField("Legal First / Given Name");
                    csvWriter.WriteField("Legal Last / Family Name");
                    csvWriter.WriteField("Email Address");
                    csvWriter.WriteField("Password");
                    csvWriter.WriteField("Confirm Password");
                    csvWriter.WriteField("Security Question 1");
                    csvWriter.WriteField("Security Answer 1");
                    csvWriter.WriteField("Security Question 2");
                    csvWriter.WriteField("Security Answer 2");
                    csvWriter.WriteField("Security Question 3");
                    csvWriter.WriteField("Security Answer 3");
                    csvWriter.WriteField("Status");
                    csvWriter.NextRecord();

                    foreach (var result in results)
                    {
                        csvWriter.WriteField(result["Legal First / Given Name"]);
                        csvWriter.WriteField(result["Legal Last / Family Name"]);
                        csvWriter.WriteField(result["Email Address"]);
                        csvWriter.WriteField(result["Password"]);
                        csvWriter.WriteField(result["Confirm Password"]);
                        csvWriter.WriteField(result["Security Question 1"]);
                        csvWriter.WriteField(result["Security Answer 1"]);
                        csvWriter.WriteField(result["Security Question 2"]);
                        csvWriter.WriteField(result["Security Answer 2"]);
                        csvWriter.WriteField(result["Security Question 3"]);
                        csvWriter.WriteField(result["Security Answer 3"]);
                        csvWriter.WriteField(result["Status"]);
                        csvWriter.NextRecord();
                    }
                }
            }
        }
    }
}