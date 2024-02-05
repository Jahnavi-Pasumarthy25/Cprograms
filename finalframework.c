#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MENU_FILE "Menu.cfg"
#define FIELD_FILE "Fields.cfg"
#define DATA_FILE "Framework.dat"

#define LINE_LENGTH 100
#define FIELD_LENGTH 20

FILE *FpMenu;
FILE *FpField;
FILE *FpData;

char **FieldName;
char RecordId[FIELD_LENGTH];
int FieldCount;

int GetFieldCount()
{
    FieldCount = 0;
    char FieldName[LINE_LENGTH];
    FpField = fopen(FIELD_FILE, "r");
    while (fgets(FieldName, LINE_LENGTH, FpField))
    {
        FieldCount++;
    }
    fclose(FpField);
    return FieldCount;
}

char **GetFieldArray()
{
    int FieldCounter;
    FpField = fopen(FIELD_FILE, "r");
    FieldCount = GetFieldCount();
    rewind(FpField);
    FieldName = (char **)malloc(FieldCount * sizeof(char *));
    for (FieldCounter = 0; FieldCounter < FieldCount; FieldCounter++)
    {
        FieldName[FieldCounter] = (char *)malloc(LINE_LENGTH * sizeof(char));
        fgets(FieldName[FieldCounter], LINE_LENGTH, FpField);
        FieldName[FieldCounter][strlen(FieldName[FieldCounter]) - 1] = '\0';
    }
    fclose(FpField);
    return FieldName;
}

char *GetFieldData(char FieldName[])
{
    char *FieldData = (char *)malloc(FIELD_LENGTH * sizeof(char));
    printf("Please enter %s: ", FieldName);
    scanf(" %[^\n]s", FieldData);
    return FieldData;
}

void Showmenu()
{
    FpMenu = fopen(MENU_FILE, "r");
    char MenuLine[LINE_LENGTH];
    while (fgets(MenuLine, LINE_LENGTH, FpMenu))
    {
        printf("%s", MenuLine);
    }
    printf("\n");
    fclose(FpMenu);
}

void PrintfCreateStatus(int FieldCounter)
{
    if (FieldCounter == FieldCount)
    {
        printf("Created a record successfully.\n");
    }
    else
    {
        printf("Failed to create a record.\n");
    }
}

void CreateNewRecord()
{
    char *FieldValue;
    rewind(FpField);
    int CountOfFieldsAdded = 0, FieldCounter;
    FpData = fopen(DATA_FILE, "a");

    for (FieldCounter = 0; FieldCounter < FieldCount; FieldCounter++)
    {
        FieldValue = GetFieldData(FieldName[FieldCounter]);
        CountOfFieldsAdded = CountOfFieldsAdded + fwrite(FieldValue, FIELD_LENGTH, 1, FpData);
    }
    
    CountOfFieldsAdded = CountOfFieldsAdded + fwrite("A", sizeof(char), 1, FpData);
    fclose(FpData);
    PrintfCreateStatus(CountOfFieldsAdded);
}

void ShowAllRecords()
{
    char Record[FIELD_LENGTH];
    int FieldCounter;

    FpData = fopen(DATA_FILE, "r");
    while (fread(Record, FIELD_LENGTH, 1, FpData))
    {
        if (Record[FieldCount - 1] == 'A')
        {
            for (FieldCounter = 0; FieldCounter < FieldCount; FieldCounter++)
            {
                printf("%s: %s\n", FieldName[FieldCounter], Record);
                fread(Record, FIELD_LENGTH, 1, FpData);
            }
            printf("-----------------------------------------\n");
        }
    }
    fclose(FpData);
}

char *GetRecordId()
{
    printf("Enter %s: ", FieldName[0]);
    scanf("%s", RecordId);
    return RecordId;
}

int GetRecordPosition(char RecordId[])
{
    char Record[FIELD_LENGTH];
    int RecordPosition = -1;
    FpData = fopen(DATA_FILE, "r");
    while (fread(Record, FIELD_LENGTH, 1, FpData))
    {
        if ((Record[FieldCount - 1] == 'A') && (strcmp(Record, RecordId) == 0))
        {
            RecordPosition = ftell(FpData) - (FIELD_LENGTH * FieldCount);
            break;
        }
    }
    fclose(FpData);
    return RecordPosition;
}

void SearchRecord(char RecordId[])
{
    char Record[FIELD_LENGTH];
    int FieldCounter;
    int RecordPosition = GetRecordPosition(RecordId);

    if (RecordPosition != -1)
    {
        FpData = fopen(DATA_FILE, "r");
        fseek(FpData, RecordPosition, SEEK_SET);
        printf("Details of record %s: ", RecordId);
        for (FieldCounter = 0; FieldCounter < FieldCount; FieldCounter++)
        {
            fread(Record, FIELD_LENGTH, 1, FpData);
            printf("%s: %s\n", FieldName[FieldCounter], Record);
        }
    }
    else
    {
        printf("Record %s is not found!\n", RecordId);
    }
    fclose(FpData);
}

char *GetNewValue()
{
    char *NewValue = (char *)malloc(FIELD_LENGTH * sizeof(char));
    printf("Enter new %s: ", FieldName[FieldCount - 1]);
    scanf(" %[^\n]s", NewValue);
    return NewValue;
}

void PrintUpdateStatus(char RecordId[], int Updated)
{
    if (Updated)
    {
        printf("Updated record %s successfully.\n", RecordId);
    }
    else
    {
        printf("Failed to update record %s!\n", RecordId);
    }
}

void UpdateRecord(char RecordId[])
{
    char Record[FIELD_LENGTH];
    int FieldCounter;
    int RecordPosition = GetRecordPosition(RecordId);
    char *NewValue;
    int Updated = 0;
    if (RecordPosition != -1)
    {
        NewValue = GetNewValue();
        FpData = fopen(DATA_FILE, "r+");
        fseek(FpData, RecordPosition, SEEK_SET);
        for (FieldCounter = 0; FieldCounter < FieldCount - 1; FieldCounter++)
        {
            fread(Record, FIELD_LENGTH, 1, FpData);
        }
        strcpy(Record, NewValue);
        fseek(FpData, -FIELD_LENGTH, SEEK_CUR);
        Updated = fwrite(Record, FIELD_LENGTH, 1, FpData);
        fclose(FpData);
    }
    PrintUpdateStatus(RecordId, Updated);
}

void PrintDeletestatus(char RecordId[], int Deleted)
{
    if (Deleted)
    {
        printf("Record %s is deleted successfully.\n", RecordId);
    }
    else
    {
        printf("Failed to delete the record %s!\n", RecordId);
    }
}

void DeleteRecord(char RecordId[])
{
    char Record[FIELD_LENGTH];
    int RecordPosition = GetRecordPosition(RecordId);
    int Deleted = 0;
    if (RecordPosition != -1)
    {
        FpData = fopen(DATA_FILE, "r+");
        fseek(FpData, RecordPosition, SEEK_SET);
        fread(Record, FIELD_LENGTH, 1, FpData);
        Record[FieldCount - 1] = 'D';
        fseek(FpData, -sizeof(char), SEEK_CUR);
        Deleted = fwrite(Record, FIELD_LENGTH, 1, FpData);
        fclose(FpData);
    }
    PrintDeletestatus(RecordId, Deleted);
}

void ChooseOption()
{
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 0:
        exit(0);
        break;
    case 1:
        CreateNewRecord();
        break;
    case 2:
        ShowAllRecords();
        break;
    case 3:
        UpdateRecord(GetRecordId());
        break;
    case 4:
        DeleteRecord(GetRecordId());
        break;
    case 5:
        SearchRecord(GetRecordId());
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }
}

int main()
{
    GetFieldCount();
    GetFieldArray();

    while (1)
    {
        Showmenu();
        ChooseOption();
    }

    return 0;
}
