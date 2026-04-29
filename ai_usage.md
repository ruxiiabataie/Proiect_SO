For the filter command, I used ChatGPT only for the two functions required in the project: parse_condition() and match_condition().
The project is a C program called city_manager, where reports about city problems like damaged roads, broken lights or flooding are stored in a binary file called reports.dat.
The filter command is used to search reports using conditions like:
severity:>=:2
category:==:road
inspector:!=:alice
For the first function, parse_condition(), I asked ChatGPT for a function that takes a string like severity:>=:2 and splits it into 3 parts: field, operator and value.
It generated this:
int parse_condition(const char *input, char *field, char *op, char *value) {
    return sscanf(input, "%[^:]:%[^:]:%s", field, op, value);
}
I kept it exactly like that because it was simple and correct. I tested it with a few examples and it worked without problems.
The second function was match_condition().
Here I asked for a function that checks if a report matches a condition depending on the field (severity, category, inspector, timestamp) and the operator (==, !=, <, >, <=, >=).
It also generated a helper function called match_int() for comparing numbers.
For example, if the condition is:
severity:>=:2
the function checks if the severity from the report is greater than or equal to 2.
If the condition is:
category:==:road
it compares the category from the report with "road" using strcmp().
For inspector, it works the same way, and for timestamp, it converts the value to integer and compares it as a number.
I kept most of this code too because it was already good, but I checked it line by line to make sure I really understood it.
For example:
atoi() changes values like "2" from text into integers
strcmp() is used for comparing strings like category and inspector
match_int() makes the numeric comparisons cleaner and avoids repeating code
I tested the filter command with real examples like:
severity:>=:2
category:==:road
and checked that only the correct reports were displayed.