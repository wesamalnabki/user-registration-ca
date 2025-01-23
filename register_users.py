import csv
import random
import string
import argparse
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import Select
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

BASE_URL = "https://account.ocas.ca/en-CA/Account/Register"

# Function to generate a random password
def generate_password(length=12):
    characters = string.ascii_letters + string.digits + string.punctuation
    return ''.join(random.choice(characters) for _ in range(length))

def execute(input_data):
    # Generate a random password
    password = generate_password()
    input_data["Password"] = password
    input_data["Confirm Password"] = password

    # Initialize the WebDriver (e.g., Chrome)
    driver = webdriver.Chrome()  # Ensure chromedriver is in your PATH
    driver.get(BASE_URL)

    try:
        # Wait for the page to load
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "firstName")))

        # Fill in the form fields
        driver.find_element(By.ID, "firstName").send_keys(input_data["Legal First / Given Name"])
        driver.find_element(By.ID, "lastName").send_keys(input_data["Legal Last / Family Name"])
        driver.find_element(By.ID, "email").send_keys(input_data["Email Address"])
        driver.find_element(By.ID, "password").send_keys(input_data["Password"])
        driver.find_element(By.ID, "confirmPassword").send_keys(input_data["Confirm Password"])

        # Fill in security questions and answers
        security_questions = input_data.get("Security Questions", [])
        for i in range(3):
            question_id = f"securityQuestion{i}Id"
            answer_id = f"securityQuestion{i}Answer"

            # Get the dropdown element
            question_dropdown = Select(driver.find_element(By.ID, question_id))
            options = [option.text for option in question_dropdown.options]

            # Check if the provided question exists in the dropdown
            if i < len(security_questions):
                provided_question = security_questions[i]["Question"]
                provided_answer = security_questions[i]["Answer"]

                if provided_question in options:
                    # Select the provided question
                    question_dropdown.select_by_visible_text(provided_question)
                    # Fill in the provided answer
                    driver.find_element(By.ID, answer_id).send_keys(provided_answer)
                else:
                    # If the question is not found, select the first option and use the provided answer
                    question_dropdown.select_by_index(1)
                    driver.find_element(By.ID, answer_id).send_keys(provided_answer)
            else:
                # If no question is provided, select the first option and generate a random answer
                question_dropdown.select_by_index(1)
                random_answer = ''.join(random.choice(string.ascii_letters) for _ in range(10))
                driver.find_element(By.ID, answer_id).send_keys(random_answer)

        # Tick the consent checkbox by clicking the label
        privacy_label = driver.find_element(By.CSS_SELECTOR, "label[for='privacyStatement']")
        privacy_label.click()

        # Click the "Create" button (commented out for testing)
        # driver.find_element(By.CSS_SELECTOR, "button[type='submit']").click()

        # Mark the status as success
        input_data["Status"] = "Success"
        return input_data

    except Exception as e:
        # Mark the status as failed if an exception occurs
        input_data["Status"] = "Failed"
        return input_data

    finally:
        # Close the browser
        driver.quit()


def process_csv(input_csv_path, output_csv_path):
    # Read the input CSV file
    with open(input_csv_path, mode="r", newline="", encoding="utf-8") as csvfile:
        reader = csv.DictReader(csvfile)
        rows = list(reader)

    # Process each row
    results = []
    for row in rows:
        # Convert the row to the expected input format
        input_data = {
            "Legal First / Given Name": row["Legal First / Given Name"],
            "Legal Last / Family Name": row["Legal Last / Family Name"],
            "Email Address": row["Email Address"],
            "Security Questions": [
                {"Question": row["Security Question 1"], "Answer": row["Security Answer 1"]},
                {"Question": row["Security Question 2"], "Answer": row["Security Answer 2"]},
                {"Question": row["Security Question 3"], "Answer": row["Security Answer 3"]}
            ]
        }

        # Execute the registration process
        result = execute(input_data)
        results.append(result)

    # Write the results to the output CSV file
    with open(output_csv_path, mode="w", newline="", encoding="utf-8") as csvfile:
        fieldnames = [
            "Legal First / Given Name", "Legal Last / Family Name", "Email Address",
            "Password", "Confirm Password",
            "Security Question 1", "Security Answer 1",
            "Security Question 2", "Security Answer 2",
            "Security Question 3", "Security Answer 3",
            "Status"
        ]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        for result in results:
            writer.writerow({
                "Legal First / Given Name": result["Legal First / Given Name"],
                "Legal Last / Family Name": result["Legal Last / Family Name"],
                "Email Address": result["Email Address"],
                "Password": result["Password"],
                "Confirm Password": result["Confirm Password"],
                "Security Question 1": result["Security Questions"][0]["Question"],
                "Security Answer 1": result["Security Questions"][0]["Answer"],
                "Security Question 2": result["Security Questions"][1]["Question"],
                "Security Answer 2": result["Security Questions"][1]["Answer"],
                "Security Question 3": result["Security Questions"][2]["Question"],
                "Security Answer 3": result["Security Questions"][2]["Answer"],
                "Status": result["Status"]
            })


if __name__ == "__main__":
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Automate registration using data from a CSV file.")
    parser.add_argument("--input", required=True, help="Path to the input CSV file.")
    parser.add_argument("--output", required=True, help="Path to the output CSV file.")
    args = parser.parse_args()

    # Process the CSV file
    process_csv(args.input, args.output)
    # process_csv("input.csv", "output.csv")
    print(f"Results have been written to {args.output}")