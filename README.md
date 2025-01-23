# Automated Registration Script

This Python script automates the registration process on a webpage using Selenium. It reads user data from a CSV file, fills out the registration form, and writes the results (including generated passwords and status) to an output CSV file.

## Features
- Reads user data from a CSV file.
- Automates form filling using Selenium.
- Generates random passwords for each user.
- Handles security questions and answers.
- Adds a `Status` column to indicate success or failure.
- Writes results to an output CSV file.

## Prerequisites
- Python 3.x
- Chrome browser (or any other browser supported by Selenium)
- ChromeDriver (or the appropriate WebDriver for your browser)

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/automated-registration.git
   cd automated-registration

2. **Set up a virtual environment (optional but recommended)**:
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate

3. **Install dependencies**:
   ```bash
   pip install -r requirements.txt

## Usage

### Input CSV Format
The input CSV file should have the following columns:
- `Legal First / Given Name`
- `Legal Last / Family Name`
- `Email Address`
- `Security Question 1`, `Security Answer 1`
- `Security Question 2`, `Security Answer 2`
- `Security Question 3`, `Security Answer 3`

Example (`input.csv`):
```csv
Legal First / Given Name,Legal Last / Family Name,Email Address,Security Question 1,Security Answer 1,Security Question 2,Security Answer 2,Security Question 3,Security Answer 3
John,Doe,johndoe@example.com,What city was your father born in?,damascus,What is your mother's middle name?,rama,What is the name of the street you grew up on?,El Musel street
Jane,Smith,janesmith@example.com,What is your favourite teacher's name?,Mr. Brown,What city was your father born in?,Toronto,What is your mother's middle name?,Marie
```

### Run the Script
Run the script with the `--input` and `--output` arguments to specify the input and output CSV files:
```bash
python register_users.py --input input.csv --output output.csv
```
