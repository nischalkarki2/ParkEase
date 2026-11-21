#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/server.h"
#include "../include/parking.h"
#include "../include/linkedlist.h"
#include "../include/queue.h"
#include "../include/stack.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SocketType;
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int SocketType;
    #define CLOSE_SOCKET close
#endif

// Receive the complete HTTP request, including its body
int receiveHttpRequest(
    SocketType clientSocket,
    char *buffer,
    size_t bufferSize
)
{
    size_t totalReceived = 0;
    char *headerEnd = NULL;

    // Receive data until the HTTP headers are complete
    while (headerEnd == NULL)
    {
        if (totalReceived >= bufferSize - 1)
            return -1;

        int received = recv(
            clientSocket,
            buffer + totalReceived,
            (int)(bufferSize - 1 - totalReceived),
            0
        );

        if (received <= 0)
            return -1;

        totalReceived += (size_t)received;
        buffer[totalReceived] = '\0';

        headerEnd = strstr(buffer, "\r\n\r\n");
    }

    // Calculate the length of the HTTP headers
    size_t headerLength = (size_t)(headerEnd - buffer) + 4;

    // Find the Content-Length header
    int contentLength = 0;
    char *lengthHeader = strstr(buffer, "Content-Length:");

    if (lengthHeader != NULL)
    {
        char *valueStart = strchr(lengthHeader, ':') + 1;
        char *endPointer;

        long value = strtol(valueStart, &endPointer, 10);

        if (endPointer == valueStart ||
            value < 0 ||
            value > (long)(bufferSize - headerLength - 1))
        {
            return -1;
        }

        contentLength = (int)value;
    }

    // Receive the remaining request body
    while (totalReceived < headerLength + (size_t)contentLength)
    {
        int received = recv(
            clientSocket,
            buffer + totalReceived,
            (int)(bufferSize - 1 - totalReceived),
            0
        );

        if (received <= 0)
            return -1;

        totalReceived += (size_t)received;
    }

    buffer[totalReceived] = '\0';

    return (int)totalReceived;
}

// Extract a string value from a JSON object
int extractJSONString(
    const char *json,
    const char *key,
    char *output,
    size_t outputSize
)
{
    char searchKey[64];

    snprintf(
        searchKey,
        sizeof(searchKey),
        "\"%s\"",
        key
    );

    const char *position = strstr(json, searchKey);

    if (position == NULL)
    {
        return 0;
    }

    position = strchr(position, ':');

    if (position == NULL)
    {
        return 0;
    }

    position++;

    while (*position == ' ' || *position == '\t')
    {
        position++;
    }

    if (*position != '"')
    {
        return 0;
    }

    position++;

    size_t length = 0;

    while (*position != '\0' && *position != '"')
    {
        if (*position == '\\' || length >= outputSize - 1)
        {
            return 0;
        }

        output[length++] = *position++;
    }

    if (*position != '"')
    {
        return 0;
    }

    output[length] = '\0';

    return 1;
}


// Extract an integer value from a JSON object
int extractJSONInt(
    const char *json,
    const char *key,
    int *output
)
{
    char searchKey[64];

    snprintf(
        searchKey,
        sizeof(searchKey),
        "\"%s\"",
        key
    );

    const char *position = strstr(json, searchKey);

    if (position == NULL)
    {
        return 0;
    }

    position = strchr(position, ':');

    if (position == NULL)
    {
        return 0;
    }

    position++;

    while (*position == ' ' || *position == '\t')
    {
        position++;
    }

    char *endPointer;

    long value = strtol(position, &endPointer, 10);

    if (position == endPointer)
    {
        return 0;
    }

    while (*endPointer == ' ' || *endPointer == '\t')
    {
        endPointer++;
    }

    if (*endPointer != ',' && *endPointer != '}')
    {
        return 0;
    }

    if (value < 1 || value > 2)
    {
        return 0;
    }

    *output = (int)value;

    return 1;
}

void startServer(void)
{
    SocketType serverSocket;
    struct sockaddr_in serverAddress;

        const char *portEnvironment = getenv("PORT");

    int serverPort = (portEnvironment != NULL)
        ? atoi(portEnvironment)
        : SERVER_PORT;

    if (serverPort < 1 || serverPort > 65535)
    {
        serverPort = SERVER_PORT;
    }

#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Winsock initialization failed!\n");
        return;
    }
#endif

    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
    if (serverSocket == INVALID_SOCKET)
#else
    if (serverSocket < 0)
#endif
    {
        printf("Socket creation failed!\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    // Configure the server address
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(serverPort);

    // Bind the socket to port 8080
    if (bind(serverSocket, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0)
    {
        printf("Socket binding failed! Port may already be in use.\n");
        CLOSE_SOCKET(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    // Start listening for connections
    if (listen(serverSocket, 10) < 0)
    {
        printf("Server listening failed!\n");
        CLOSE_SOCKET(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    printf("\n====================================\n");
    printf("       PARKEASE HTTP SERVER\n");
    printf("====================================\n");
    printf("Server listening on port %d\n", serverPort);
    printf("Waiting for frontend connections...\n");


    // Accept incoming client connections
    while (1)
    {
        SocketType clientSocket;
        char requestBuffer[BUFFER_SIZE];

        clientSocket = accept(serverSocket, NULL, NULL);

#ifdef _WIN32
        if (clientSocket == INVALID_SOCKET)
#else
        if (clientSocket < 0)
#endif
        {
            printf("Failed to accept client connection!\n");
            continue;
        }

        // Receive the HTTP request
        int bytesReceived = receiveHttpRequest(
    clientSocket,
    requestBuffer,
    sizeof(requestBuffer)
);

if (bytesReceived <= 0)
{
    CLOSE_SOCKET(clientSocket);
    continue;
}

requestBuffer[bytesReceived] = '\0';

        if (bytesReceived > 0)
        {
            requestBuffer[bytesReceived] = '\0';

            printf("\nHTTP Request Received:\n");
            printf("%s\n", requestBuffer);

            /// Handle browser CORS preflight requests
            if (strncmp(requestBuffer, "OPTIONS ", 8) == 0)
            {
                const char *response =
                    "HTTP/1.1 204 No Content\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                    "Access-Control-Allow-Headers: Content-Type\r\n"
                    "Access-Control-Max-Age: 86400\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                send(clientSocket, response, (int)strlen(response), 0);
            }
            else
            {
    // Keep the existing temporary JSON response code here
            // Prepare the API response
            const char *responseBody;
            const char *statusLine = "HTTP/1.1 200 OK\r\n";
            char jsonBody[BUFFER_SIZE];
            
            if (strncmp(requestBuffer, "POST /api/vehicles ", 19) == 0)
        {
            // Find the beginning of the JSON request body
            char *requestBody = strstr(requestBuffer, "\r\n\r\n");

            if (requestBody == NULL)
            {
                statusLine = "HTTP/1.1 400 Bad Request\r\n";

                responseBody =
                    "{\"status\":\"error\","
                    "\"message\":\"Invalid HTTP request!\"}";
            }
            else
            {
                requestBody += 4;

                char vehicleNumber[20];
                char ownerName[50];
                int vehicleType;

                int validNumber = extractJSONString(
                    requestBody,
                    "vehicleNumber",
                    vehicleNumber,
                    sizeof(vehicleNumber)
                );

                int validOwner = extractJSONString(
                    requestBody,
                    "ownerName",
                    ownerName,
                    sizeof(ownerName)
                );

                int validType = extractJSONInt(
                    requestBody,
                    "vehicleType",
                    &vehicleType
                );

                if (!validNumber || !validOwner || !validType)
                {
                    statusLine = "HTTP/1.1 400 Bad Request\r\n";

                    responseBody =
                        "{\"status\":\"error\","
                        "\"message\":\"Invalid vehicle information!\"}";
                }
                else
                {
                    int assignedSlot;
                    int isWaiting;

                    if (registerVehicleAPI(
                            vehicleNumber,
                            ownerName,
                            vehicleType,
                            &assignedSlot,
                            &isWaiting))
                    {
                        if (isWaiting)
                        {
                            responseBody =
                                "{\"status\":\"success\","
                                "\"message\":\"Vehicle added to waiting queue!\","
                                "\"isWaiting\":true}";
                        }
                        else
                        {
                            snprintf(
                                jsonBody,
                                sizeof(jsonBody),
                                "{\"status\":\"success\","
                                "\"message\":\"Vehicle parked successfully!\","
                                "\"isWaiting\":false,"
                                "\"slotNumber\":%d}",
                                assignedSlot
                            );

                            responseBody = jsonBody;
                        }
                    }
                    else
                    {
                        statusLine = "HTTP/1.1 400 Bad Request\r\n";

                        responseBody =
                            "{\"status\":\"error\","
                            "\"message\":\"Registration failed. Check vehicle details, duplicate registration, or queue capacity.\"}";
                    }
                }
            }
        }   
            
            else if (strncmp(requestBuffer, "DELETE /api/vehicles ", 21) == 0)
            {
                // Find the beginning of the JSON request body
                char *requestBody = strstr(requestBuffer, "\r\n\r\n");

                if (requestBody == NULL)
                {
                    statusLine = "HTTP/1.1 400 Bad Request\r\n";

                    responseBody =
                        "{\"status\":\"error\","
                        "\"message\":\"Invalid HTTP request!\"}";
                }
                else
                {
                    requestBody += 4;

                    char vehicleNumber[20];

                    int validNumber = extractJSONString(
                        requestBody,
                        "vehicleNumber",
                        vehicleNumber,
                        sizeof(vehicleNumber)
                    );

                    if (!validNumber)
                    {
                        statusLine = "HTTP/1.1 400 Bad Request\r\n";

                        responseBody =
                            "{\"status\":\"error\","
                            "\"message\":\"Vehicle number is required!\"}";
                    }
                    else
                    {
                        // Convert vehicle number to uppercase
                        for (int i = 0; vehicleNumber[i] != '\0'; i++)
                        {
                            vehicleNumber[i] =
                                (char)toupper((unsigned char)vehicleNumber[i]);
                        }

                        int freedSlot;
                        int wasPromoted;

                        if (removeParkedVehicleAPI(
                                vehicleNumber,
                                &freedSlot,
                                &wasPromoted))
                        {
                            snprintf(
                                jsonBody,
                                sizeof(jsonBody),
                                "{\"status\":\"success\","
                                "\"message\":\"Vehicle removed successfully!\","
                                "\"freedSlot\":%d,"
                                "\"wasPromoted\":%s}",
                                freedSlot,
                                wasPromoted ? "true" : "false"
                            );

                            responseBody = jsonBody;
                        }
                        else
                        {
                            statusLine = "HTTP/1.1 400 Bad Request\r\n";

                            responseBody =
                                "{\"status\":\"error\","
                                "\"message\":\"Vehicle not found or removal failed!\"}";
                        }
                    }
                }
            }

            else if (strncmp(requestBuffer, "GET /api/health", 15) == 0)
            {
                responseBody =
                    "{\"status\":\"success\","
                    "\"message\":\"ParkEase backend is healthy!\"}";
            }
            else if (strncmp(requestBuffer, "GET /api/status", 15) == 0)
            {
                int twoWheelerOccupied;
                int fourWheelerOccupied;

                getParkingStatistics(
                    &twoWheelerOccupied,
                    &fourWheelerOccupied
                );

                int twoWheelerAvailable =
                    TWO_WHEELER_SLOTS - twoWheelerOccupied;

                int fourWheelerAvailable =
                    FOUR_WHEELER_SLOTS - fourWheelerOccupied;

                snprintf(
                    jsonBody,
                    sizeof(jsonBody),
                    "{\"status\":\"success\","
                    "\"parking\":{"
                    "\"twoWheeler\":{"
                    "\"totalSlots\":%d,"
                    "\"occupiedSlots\":%d,"
                    "\"availableSlots\":%d"
                    "},"
                    "\"fourWheeler\":{"
                    "\"totalSlots\":%d,"
                    "\"occupiedSlots\":%d,"
                    "\"availableSlots\":%d"
                    "}"
                    "}"
                    "}",
                    TWO_WHEELER_SLOTS,
                    twoWheelerOccupied,
                    twoWheelerAvailable,
                    FOUR_WHEELER_SLOTS,
                    fourWheelerOccupied,
                    fourWheelerAvailable
                );

                responseBody = jsonBody;
            }
            else if (strncmp(requestBuffer, "GET /api/vehicles ", 17) == 0)
            {
                int vehicleCount = 0;

                Vehicle *current = vehicleHead;

                // Count parked vehicles
                while (current != NULL)
                {
                    vehicleCount++;
                    current = current->next;
                }

                // Start JSON response
                int written = snprintf(
                    jsonBody,
                    sizeof(jsonBody),
                    "{\"status\":\"success\","
                    "\"count\":%d,"
                    "\"vehicles\":[",
                    vehicleCount
                );

                size_t offset = (written > 0) ? (size_t)written : 0;

                current = vehicleHead;

                // Add each parked vehicle
                while (current != NULL && offset < sizeof(jsonBody))
                {
                    written = snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "%s{"
                        "\"vehicleNumber\":\"%s\","
                        "\"ownerName\":\"%s\","
                        "\"vehicleType\":%d,"
                        "\"slotNumber\":%d,"
                        "\"entryTime\":\"%s\""
                        "}",
                        (current == vehicleHead) ? "" : ",",
                        current->info.vehicleNumber,
                        current->info.ownerName,
                        current->info.vehicleType,
                        current->slotNumber,
                        current->entryTime
                    );

                    if (written < 0 || (size_t)written >= sizeof(jsonBody) - offset)
                    {
                        break;
                    }

                    offset += (size_t)written;
                    current = current->next;
                }

                // Finish JSON response
                if (current == NULL)
                {
                    snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "]}"
                    );

                    responseBody = jsonBody;
                }
                else
                {
                    statusLine = "HTTP/1.1 500 Internal Server Error\r\n";

                    responseBody =
                        "{\"status\":\"error\","
                        "\"message\":\"Vehicle response is too large!\"}";
                }
            }
            else if (strncmp(requestBuffer, "GET /api/queues ", 15) == 0)
            {
                int written = snprintf(
                    jsonBody,
                    sizeof(jsonBody),
                    "{\"status\":\"success\","
                    "\"queues\":{"
                    "\"twoWheeler\":{"
                    "\"count\":%d,"
                    "\"vehicles\":[",
                    twoWheelerQueue.count
                );

                size_t offset = (written > 0) ? (size_t)written : 0;

                // Add two-wheeler waiting vehicles
                for (int i = 0; i < twoWheelerQueue.count; i++)
                {
                    int index = (twoWheelerQueue.front + i) % MAX_QUEUE;

                    written = snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "%s{"
                        "\"vehicleNumber\":\"%s\","
                        "\"ownerName\":\"%s\","
                        "\"vehicleType\":%d"
                        "}",
                        (i == 0) ? "" : ",",
                        twoWheelerQueue.vehicles[index].vehicleNumber,
                        twoWheelerQueue.vehicles[index].ownerName,
                        twoWheelerQueue.vehicles[index].vehicleType
                    );

                    if (written < 0 || (size_t)written >= sizeof(jsonBody) - offset)
                    {
                        break;
                    }

                    offset += (size_t)written;
                }

                // Continue with four-wheeler queue
                if (written >= 0 && offset < sizeof(jsonBody))
                {
                    written = snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "]},"
                        "\"fourWheeler\":{"
                        "\"count\":%d,"
                        "\"vehicles\":[",
                        fourWheelerQueue.count
                    );

                    if (written >= 0 && (size_t)written < sizeof(jsonBody) - offset)
                    {
                        offset += (size_t)written;

                        for (int i = 0; i < fourWheelerQueue.count; i++)
                        {
                            int index = (fourWheelerQueue.front + i) % MAX_QUEUE;

                            written = snprintf(
                                jsonBody + offset,
                                sizeof(jsonBody) - offset,
                                "%s{"
                                "\"vehicleNumber\":\"%s\","
                                "\"ownerName\":\"%s\","
                                "\"vehicleType\":%d"
                                "}",
                                (i == 0) ? "" : ",",
                                fourWheelerQueue.vehicles[index].vehicleNumber,
                                fourWheelerQueue.vehicles[index].ownerName,
                                fourWheelerQueue.vehicles[index].vehicleType
                            );

                            if (written < 0 || (size_t)written >= sizeof(jsonBody) - offset)
                            {
                                break;
                            }

                            offset += (size_t)written;
                        }

                        if (written >= 0 && offset < sizeof(jsonBody))
                        {
                            written = snprintf(
                                jsonBody + offset,
                                sizeof(jsonBody) - offset,
                                "]}}}"
                            );

                            if (written >= 0 && (size_t)written < sizeof(jsonBody) - offset)
                            {
                                responseBody = jsonBody;
                            }
                            else
                            {
                                written = -1;
                            }
                        }
                    }
                }

                if (written < 0)
                {
                    statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
                    responseBody =
                        "{\"status\":\"error\","
                        "\"message\":\"Queue response is too large!\"}";
                }
            }
            else if (strncmp(requestBuffer, "GET /api/activities ", 19) == 0)
            {
                int activityCount = activityStack.top + 1;

                int written = snprintf(
                    jsonBody,
                    sizeof(jsonBody),
                    "{\"status\":\"success\","
                    "\"count\":%d,"
                    "\"activities\":[",
                    activityCount
                );

                size_t offset = (written > 0) ? (size_t)written : 0;

                // Return newest activities first
                for (int i = activityStack.top;
                    i >= 0 && offset < sizeof(jsonBody);
                    i--)
                {
                    written = snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "%s\"%s\"",
                        (i == activityStack.top) ? "" : ",",
                        activityStack.operations[i]
                    );

                    if (written < 0 || (size_t)written >= sizeof(jsonBody) - offset)
                    {
                        break;
                    }

                    offset += (size_t)written;
                }

                // Finish JSON response
                if (written >= 0 && offset < sizeof(jsonBody))
                {
                    written = snprintf(
                        jsonBody + offset,
                        sizeof(jsonBody) - offset,
                        "]}"
                    );

                    if (written >= 0 && (size_t)written < sizeof(jsonBody) - offset)
                    {
                        responseBody = jsonBody;
                    }
                    else
                    {
                        written = -1;
                    }
                }

                if (written < 0)
                {
                    statusLine = "HTTP/1.1 500 Internal Server Error\r\n";

                    responseBody =
                        "{\"status\":\"error\","
                        "\"message\":\"Activity response is too large!\"}";
                }
            }
            else
            {
                statusLine = "HTTP/1.1 404 Not Found\r\n";

                responseBody =
                    "{\"status\":\"error\","
                    "\"message\":\"API endpoint not found!\"}";
            }

            char response[BUFFER_SIZE + 512];

            int responseLength = snprintf(
                response,
                sizeof(response),
                "%s"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s",
                statusLine,
                strlen(responseBody),
                responseBody
            );

            if (responseLength > 0 && responseLength < (int)sizeof(response))
            {
                send(clientSocket, response, responseLength, 0);
            }

        // Close the client connection
        CLOSE_SOCKET(clientSocket);
    }
    }
}
}