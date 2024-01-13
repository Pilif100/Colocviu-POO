#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <exception>
#include <cstring>
#include <bits/stdc++.h>
using namespace std;

class Step
{
public:
    virtual void execute() = 0;
    virtual ~Step() {}
};
class TitleStep : public Step
{
private:
    string title, subtitle;

public:
    TitleStep(const  string& title, const  string& subtitle)
        : title(title), subtitle(subtitle) {}

    void execute() override
    {
        cout << "Title: " << title << ", Subtitle: " << subtitle <<  endl;
    }
};
class TextStep : public Step
{
private:
    string title, copy;

public:
    TextStep(const  string& title, const  string& copy)
        : title(title), copy(copy) {}

    void execute() override
    {
        cout << "Title: " << title << "\nText: " << copy <<  endl;
    }
};
class NumberInputStep : public Step
{
private:
    string description;
    float numberInput;

public:
    NumberInputStep(const  string& description) : description(description), numberInput(0.0f) {}

    void execute() override
    {
        cout << description << ": ";
        while (!( cin >> numberInput))
        {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore( numeric_limits< streamsize>::max(), '\n');
        }
        cin.ignore( numeric_limits< streamsize>::max(), '\n');
        cout << "You entered: " << numberInput <<  endl;
    }

    float getNumberInput() const
    {
        return numberInput;
    }
};
class CalculusStep : public Step
{
private:
    vector<NumberInputStep*> inputs;
    char operation;

public:
    CalculusStep(const  vector<NumberInputStep*>& inputs, char operation)
        : inputs(inputs), operation(operation) {}

    void execute() override
    {
        if (inputs.empty())
        {
            cout << "CalculusStep error: No inputs provided." <<  endl;
            return;
        }

        float result = inputs[0]->getNumberInput();
        for (size_t i = 1; i < inputs.size(); ++i)
        {
            float nextInput = inputs[i]->getNumberInput();
            switch (operation)
            {
            case '+':
                result += nextInput;
                break;
            case '-':
                result -= nextInput;
                break;
            case '*':
                result *= nextInput;
                break;
            case '/':
                if (nextInput != 0.0f)
                {
                    result /= nextInput;
                }
                else
                {
                    cout << "Error: Division by zero." <<  endl;
                    return;
                }
                break;
            case 'm':
                result =  min(result, nextInput);
                break;
            case 'M':
                result =  max(result, nextInput);
                break;
            default:
                cout << "Unknown operation." <<  endl;
                return;
            }
        }

        cout << "Calculus result: " << result <<  endl;
    }
};
class DisplayStep : public Step
{
private:
    string fileName;

    vector< string> parseCSVLine(const  string& line)
    {
        stringstream lineStream(line);
        vector< string> cells;
        string cell;
        bool inQuote = false;

        while (lineStream.good())
        {
            char c = lineStream.get();
            if (c == '"' && (lineStream.peek() == ',' || lineStream.peek() == EOF))
            {
                inQuote = !inQuote;
                lineStream.get();
            }
            else if (c == ',' && !inQuote)
            {
                cells.push_back(cell);
                cell.clear();
            }
            else if (c != EOF)
            {
                cell += c;
            }
        }
        cells.push_back(cell);

        return cells;
    }

public:
    DisplayStep(const  string& fileName) : fileName(fileName) {}

    void execute() override
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Unable to open file: " << fileName <<  endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            vector< string> cells = parseCSVLine(line);
            for (const auto& cell : cells)
            {
                cout << cell << "\t";
            }
            cout <<  endl;
        }
        file.close();
    }
};
class TextFileInputStep : public Step
{
private:
    string description;
    string fileName;

public:
    TextFileInputStep(const  string& description, const  string& fileName)
        : description(description), fileName(fileName) {}

    void execute() override
    {
        cout << description <<  endl;
        string userInput;
        getline( cin, userInput);

        ofstream file(fileName);
        if (!file.is_open())
        {
            cout << "Unable to open file: " << fileName <<  endl;
            return;
        }

        file << userInput;
        file.close();
    }
};
class CsvFileInputStep : public Step
{
private:
    string description;
    string fileName;

public:
    CsvFileInputStep(const  string& description, const  string& fileName)
        : description(description), fileName(fileName) {}

    void execute() override
    {
        cout << description <<  endl;

        ifstream file(fileName);
        if (!file.is_open())
        {
            cerr << "Unable to open file: " << fileName <<  endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            stringstream lineStream(line);
            string cell;
            while (getline(lineStream, cell, ','))
            {
                cout << cell << "\t";
            }
            cout <<  endl;
        }

        file.close();
    }
};
class OutputStep : public Step
{
private:
    string fileName;
    string title;
    string description;
    string content;
    time_t creationTime;

public:
    OutputStep(const  string& fileName, const  string& title, const  string& description, const  string& content)
        : fileName(fileName), title(title), description(description), content(content)
    {
        creationTime =  time(nullptr);
    }

    void execute() override
    {
        fstream file;
        file.open(fileName,ios::out);
        if(!file)
        {
            cout<<"Error in creating file!!!";
            return;
        }

        file << "Title: " << title << "\n";
        file << "Description: " << description << "\n";
        file << "Creation Time: " <<  ctime(&creationTime);
        file << "Content:\n" << content << "\n";
        file.close();
        cout << "Output saved to " << fileName <<  endl;
    }
};
class EndStep : public Step
{
public:
    EndStep() {}

    void execute() override
    {

        cout << "End of the flow reached." <<  endl;

    }
};
class Flow
{
private:
    string name;
    vector< unique_ptr<Step>> steps;
    time_t creationTime;
    int startCount=0;
    int completeCount=0;
    int skippedStepsCount=0;
    int errorScreensCount=0;
    int totalErrors=0;

public:
    Flow(const  string& name)
        : name(name), creationTime( time(nullptr)), startCount(0), completeCount(0), skippedStepsCount(0), errorScreensCount(0), totalErrors(0) {}

    void addStep( unique_ptr<Step> step)
    {
        if (step)
        {
            steps.push_back( move(step));
        }
    }

    void run()
    {
        startCount++;
        for (auto& step : steps)
        {
            bool stepCompleted = false;
            while (!stepCompleted)
            {
                try
                {
                    step->execute();
                    stepCompleted = true;
                }
                catch (const  exception& e)
                {
                    cerr << "An error occurred during step execution: " << e.what() <<  endl;
                    cout << "Choose action - Retry (r), Skip (s), Abort (a): ";
                    char action;
                    cin >> action;

                    switch (action)
                    {
                    case 'r':
                    case 'R':

                        break;
                    case 's':
                    case 'S':
                        stepCompleted = true;
                        break;
                    case 'a':
                    case 'A':
                        cout << "Flow execution aborted by the user." <<  endl;
                        return;
                    default:
                        cout << "Invalid choice. Please enter 'r', 's', or 'a'." <<  endl;
                        break;
                    }
                    cin.clear();
                    cin.ignore( numeric_limits< streamsize>::max(), '\n');
                }
            }
        }

        completeCount++;
        cout << "Flow completed successfully." <<  endl;
    }

    bool handleStepError()
    {
        char action;
        cout << "Choose action - Retry (r), Skip (s), Abort (a): ";
        cin >> action;

        switch (action)
        {
        case 'r':
        case 'R':
            return true;
        case 's':
        case 'S':
            skippedStepsCount++;
            return true;
        case 'a':
        case 'A':
            return false;
        default:
            cout << "Invalid choice. Aborting flow." <<  endl;
            return false;
        }
    }

    void displayAnalytics() const
    {
        cout << "Flow: " << name << "\n";
        cout << "Started: " << startCount << ", Completed: " << completeCount << "\n";
        cout << "Skipped Steps: " << skippedStepsCount << ", Error Screens: " << errorScreensCount << "\n";
        cout << "Average Errors per Completion: " << (completeCount > 0 ? static_cast<float>(totalErrors) / completeCount : 0) << "\n";
    }

    const  string& getName() const
    {
        return name;
    }
};
class Application
{
private:
    vector<Flow> flows;

    void createFlow()
    {
        string flowName;
        cout << "Enter flow name: ";
        getline( cin, flowName);

        Flow newFlow(flowName);
        addStepsToFlow(newFlow);

        flows.push_back( move(newFlow));
        cout << "Flow created successfully." <<  endl;
    }

    void addStepsToFlow(Flow& flow)
    {
        bool addingSteps = true;
        while (addingSteps)
        {
            cout << "\nAvailable Step Types:\n";
            cout << "1. Title Step\n2. Text Step\n3. Number Input Step\n";
            cout << "4. Calculus Step\n5. Display Step\n6. Text File Input Step\n";
            cout << "7. CSV File Input Step\n8. Output Step\n9. End Step\n";
            cout << "Enter step type to add (or 0 to finish adding steps): ";
            int choice;
            cin >> choice;
            cin.ignore();

            switch (choice)
            {
            case 1:
                addTitleStep(flow);
                break;
            case 2:
                addTextStep(flow);
                break;
            case 3:
                addNumberInputStep(flow);
                break;
            case 4:
                addCalculusStep(flow);
                break;
            case 5:
                addDisplayStep(flow);
                break;
            case 6:
                addTextFileInputStep(flow);
                break;
            case 7:
                addCsvFileInputStep(flow);
                break;
            case 8:
                addOutputStep(flow);
                break;
            case 9:
                flow.addStep( make_unique<EndStep>());
                break;
            case 0:
                addingSteps = false;
                break;
            default:
                cout << "Invalid choice." <<  endl;
            }
        }
    }

    void addTitleStep(Flow& flow)
    {
        string title, subtitle;
        cout << "Enter title: ";
        getline( cin, title);
        cout << "Enter subtitle: ";
        getline( cin, subtitle);
        flow.addStep( make_unique<TitleStep>(title, subtitle));
    }

    void addTextStep(Flow& flow)
    {
        string title, text;
        cout << "Enter text step title: ";
        getline( cin, title);
        cout << "Enter text: ";
        getline( cin, text);
        flow.addStep( make_unique<TextStep>(title, text));
    }

    void addNumberInputStep(Flow& flow)
    {
        string description;
        cout << "Enter description for number input: ";
        getline( cin, description);
        flow.addStep( make_unique<NumberInputStep>(description));
    }

    void addCalculusStep(Flow& flow)
    {
        string operation;
        cout << "Enter operation (+, -, *, /, min, max): ";
        getline( cin, operation);

        char op;
        if (operation == "+" || operation == "-" || operation == "*" ||
                operation == "/" || operation == "min" || operation == "max")
        {
            op = operation[0];
        }
        else
        {
            cout << "Invalid operation. Defaulting to addition." <<  endl;
            op = '+';
        }

        auto input1 =  make_unique<NumberInputStep>("Input 1");
        auto input2 =  make_unique<NumberInputStep>("Input 2");


        NumberInputStep* inputPtr1 = input1.get();
        NumberInputStep* inputPtr2 = input2.get();


        flow.addStep( move(input1));
        flow.addStep( move(input2));

        vector<NumberInputStep*> inputs = {inputPtr1, inputPtr2};
        flow.addStep( make_unique<CalculusStep>(inputs, op));
    }

    void addDisplayStep(Flow& flow)
    {
        string fileName;
        cout << "Enter file name to display: ";
        getline( cin, fileName);
        flow.addStep( make_unique<DisplayStep>(fileName));
    }

    void addTextFileInputStep(Flow& flow)
    {
        string description, fileName;
        cout << "Enter description: ";
        getline( cin, description);
        cout << "Enter file name: ";
        getline( cin, fileName);
        flow.addStep( make_unique<TextFileInputStep>(description, fileName));
    }

    void addCsvFileInputStep(Flow& flow)
    {
        string description, fileName;
        cout << "Enter description: ";
        getline( cin, description);
        cout << "Enter file name: ";
        getline( cin, fileName);
        flow.addStep( make_unique<CsvFileInputStep>(description, fileName));
    }

    void addOutputStep(Flow& flow)
    {
        string fileName, title, description, content;
        cout << "Enter file name: ";
        getline( cin, fileName);
        cout << "Enter title: ";
        getline( cin, title);
        cout << "Enter description: ";
        getline( cin, description);
        cout << "Enter content: ";
        getline( cin, content);
        OutputStep os(fileName, title, description, content);
        os.execute();
        flow.addStep( make_unique<OutputStep>(fileName, title, description, content));
    }

    void runFlow()
    {
        string flowName;
        cout << "Enter the flow name to run: ";
        getline( cin, flowName);

        for (auto& flow : flows)
        {
            if (flow.getName() == flowName)
            {
                flow.run();
                return;
            }
        }
        cout << "Flow not found." <<  endl;
    }

    void deleteFlow()
    {
        string flowName;
        cout << "Enter the flow name to delete: ";
        getline( cin, flowName);

        auto it =  find_if(flows.begin(), flows.end(), [&flowName](const Flow& flow)
        {
            return flow.getName() == flowName;
        });

        if (it != flows.end())
        {
            flows.erase(it);
            cout << "Flow deleted successfully." <<  endl;
        }
        else
        {
            cout << "Flow not found." <<  endl;
        }
    }

    void listFlows() const
    {
        if (flows.empty())
        {
            cout << "No flows available." <<  endl;
            return;
        }

        for (const auto& flow : flows)
        {
            flow.displayAnalytics();
        }
    }

public:
    void start()
    {
        bool isRunning = true;
        while (isRunning)
        {
            cout << "\n--- Flow Application Menu ---\n";
            cout << "1. Create Flow\n2. Run Flow\n3. Delete Flow\n4. List Flows\n5. Exit\nEnter choice: ";

            int choice;
            cin >> choice;

            if ( cin.fail())
            {
                cin.clear();
                cin.ignore( numeric_limits< streamsize>::max(), '\n');
                cout << "Invalid choice. Please enter a number." <<  endl;
            }
            else
            {
                cin.ignore( numeric_limits< streamsize>::max(), '\n');
                switch (choice)
                {
                case 1:
                    createFlow();
                    break;
                case 2:
                    runFlow();
                    break;
                case 3:
                    deleteFlow();
                    break;
                case 4:
                    listFlows();
                    break;
                case 5:
                    isRunning = false;
                    break;
                default:
                    cout << "Invalid choice." <<  endl;
                    break;
                }
            }
        }
    }
};

int main()
{
    Application app;
    app.start();
    return 0;
}

