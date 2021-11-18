# WOOTCH (Getting started, installing, deploying and using it)

<br>

---

---

---

<br>

# HOW TO INSTALL/DEPLOY WOOTCH (cloud and embedded)

## **Prerequisites**

- Git
- Node v14.18.1 (or 14 LTS latest)
- AWS Cli
- Esp-idf
- Optional: for PCB design/improvement, Altium Designer

This tutorial works for both powershell and bash.

Let's start from cloning the GitHub repository:

```bash
$ cd git/destination/folder/wootch
# Do not forget the submodule cloning parameter as this repo contains several submodules
$ git clone --recurse-submodules -j8 https://github.com/pseudoincorrect/Wootch.git
```

in this tutorial, **_{WOOTCH_PATH}_** means the absolute path to the wootch repo on your file system.

<br>

---

---

## **CLOUD**

<br>

### **Installing AWS CLI**

Install:

- https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-welcome.html

Configuration

- https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html

<br>

### **Installing Node Applications**

```powershell
$ cd {WOOTCH_PATH}/cloud/aws_cdk/build_tools
$ npm run allInstall
```

<br>

### **Creating all Secret files (needed for CDK and the two CLIs)**

create the file "secrets.ts" at the following location:

- {WOOTCH_PATH}/Cloud/aws_cdk/cli/user/secrets/

copy/paste in it the content of:

- {WOOTCH_PATH}/Cloud/aws_cdk/cli/user/secrets/secretsTemplate.ts

Follow the same process with the following folders and their respective secretsTemplate.ts files:

- {WOOTCH_PATH}/Cloud/aws_cdk/cli/admin/secrets/
- {WOOTCH_PATH}/Cloud/aws_cdk/cdk/secrets/

### **Filling the CDK secrets file**

Navigate to:

- {WOOTCH_PATH}/Cloud/aws_cdk/cdk/secrets/secrets.ts

Fill the file with the information/secrets relative to your AWS Account:

- WOOTCH_AWS_REGION : your AWS region where you intend to deploy Wootch
- WOOTCH_AWS_ACCOUNT_ID : your AWS Account ID
- "WOOTCH_COGNITO_APP_CLIENT_DOMAIN_NAME": random/empiric string/name as it should be globally unique (up to you to generate a random string)
- "SENDER_ADDRESS": with the email used to send notification to your user.
- "RECEIVER_ADDRESS": this one is for testing, use any address where your would like to receive notifications.

NOTE: For the admin and user CLIs (not for cdk), secrets.ts files will be filled later on with the values of our CDK application. However, we first need to build all lambda functions with empty secrets to enable the deployment with CDK. Once we will have these secrets (after the CDK deployment), we will update secrets.ts files for each CLI. This step can be automatised later on.

<br>

### **Building all lambda functions folders**

```powershell
$ cd {WOOTCH_PATH}/cloud/aws_cdk/build_tools
$ npm run allBuild
```

<br>

### **Deploying the CDK application**

```powershell
$ cd {WOOTCH_PATH}/cdk
$ npm run devSynth

# Check that everything is in order

$ npm run devDeploy
```

<br>

### **Filling the CLI secrets**

in the USER CLI secret fill the following by using the console:

- AWS_REGION : speaks for itself
- AWS_ACCOUNT_ID : speak for itself
- USER_POOL_ID : the cognito user pool ID
- USER_POOL_CLIENT_ID : App integration, cognito client ID of this pool
- USER_GROUP_NAME : "usersGroup"
- DOMAIN_NAME : App integration, cognito Domain name of your pool
- SIGNING_URL : provided by the template
- HOST_API : API gateway URL of your depoyed API, found in AWS console -> api gateway -> Wootch.. -> Setting -> Default Endpoint
- MOCK_USERNAME : used for testing, if you created a user, this is his email
- MOCK_PASSWORD : used for testing, if you created a user, this is his password (after password change)

The ADMIN CLI secrets are very similar to the user cli one, for the one that differ:

- DDB_TABLE_NAME : the "main" table name -> AWS console -> dynamodb -> table -> Wootch..

<br>

---

---

## HOW TO PROGRAM/SETUP THE EMBEDDED SYSTEM

Prerequisites:

- An AWS Thing must have been created with the Admin CLI
- The Certificates and Key of this thing must be available (gotten from the admin CLI)
- The Thing name must be available (gotter from the admin CLI or AWS console)

**At the moment, the certificate, key and ID are directly flashed into the device.**

Better options exists, but automating this process is not the goal of this project.

Let's install ESP-IDF:

- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html

Using a FTDI programmer (USB-to-UART) connected the corresponding FTDI headers to the system pin:

- TXDO, RXDO, DTR, RTS, GND

Navigate to {WOOTCH_PATH}/Embedded_firmware/Wootch

In the "certificate/" folder, create a "certificate.pem.crt" file and copy paste in it your device certificate:

certificate.pem.crt :

```
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJMnsSsqE1uB0dgCL1+sg7fgR0AqMA0GCSqGSIb3DQEB
...
4i8IH9J8KHsiBC30OeF37C34styAVSkR04jRBa8Ao06sKI1jOPouNIeWepDIpQ==
-----END CERTIFICATE-----
```

in the same folder, create a "private.pem.key" file and copy paste in your device private key:

private.pem.key :

```
-----BEGIN RSA PRIVATE KEY-----
MIIEpgIBAAKCAQEA7OUdkZxU5VOxL7kr3tP5AK8IHKP8DArlA1fhiNY1+mAH4MnY
...
0smW1i0AZ0+ZmsGGYVYVmkz3LwPjC5o37MyZVcBki4h4y9DTx2AbeLgg
-----END RSA PRIVATE KEY-----

```

Still in the same folder, create a "dev_id.h" file, copy paste in it the bellow code, and replace the AWS_THING_NAME macro with your device name

dev_id.h :

```C
#ifndef __dev_id_h__
#define __dev_id_h__

#define AWS_THING_NAME "WOOTCH_DEV_C8D...1B9"

#endif
```

The Idf menuconfig (sdkconfig) should be properly configured.

Navigate back to {WOOTCH_PATH}/Embedded_firmware/Wootch

```bash
# replace "COMX" by the port on which your device is connected through FTDI
$ idf.py -p COMX build flash monitor
```

<br>

---

---

## HARDWARE

Ordering and soldering is out of the scope of this tutorial.

I ordered the PCB, then soldered all components myself with the help of a hot plate, microscope/magnifier and solder paste/stencils, so it is definitely doable with generic soldering equipments.

All design files are provided to build the boards.

I had a good experience with PCBway, but any PCB maker/assembler would do the job.

<br>

---

---

---

<br>

# HOW TO USE WOOTCH

## ADMIN CLI

Navigate to {WOOTCH_PATH}/Cloud/aws_cdk/cli/admin

you can run the following command for help:

```bash
$  npm run run -- -h
```

To create a new user, with your own credentials, enter:

```bash
$  npm run run -- createUser --email userEmail@userDomain.com --password SuperStrongPassword123!
```

Then you need to change the password of this user (AWS cognito security rule)

To do so, you need the cognito app client UI link,
got to AWS console -> cognito -> Wootch user pool -> App integration -> App client settings -> Lauch Hosted UI

(this link is also found in the output of your CDK deployment)

Once this UI lauched, you can change the password of the user you created.
(this process is to be automated by mail)

To create a new device, enter:

```bash
$  npm run run -- createDevice
```

Be sure to keep the CERTIFICATE KEY, the PRIVATE KEY and the DEVICE ID (AWS THING NAME) in a text file, you will need them to program the embedded device. Without these key, you won't be able to connect the device online and these key cannot be regenerated.

## USER CLI

Navigate to {WOOTCH_PATH}/Cloud/aws_cdk/cli/user

Log into the system with the following command, with your own credentials:

```bash
$  npm run run -- login --email userEmail@userDomain.com --password SuperStrongPassword123!
```

This will generate a JWT token and store it in your file system.

To pair this user (the logged in user) to a device, you first need to generate a device pairing secret, either with the device simulator (if you want to pair to the device simulator) or with your embedded device (if you want to pair with it).

one your the 6 digit pairing secret is in your possession, enter the following command to pair with the device that generated this secret:

```bash
# replace "123456" by your secret
$  npm run -- pairing -- secret 123456
```

Now your device and your user are paired and your user will be able to receive notifications (email) depending on the device situation (sensors and alert level).

## DEVICE SIMULATOR

Fill the following secrets from {WOOTCH_PATH}/Cloud/aws_cdk/device_simulator/secrets/secrets.ts

- AWS_IOT_ENDPOINT : AWS iot-core endpoint (console -> iot-core -> settings -> endpoint )
- AWS_REGION : AWS region where Wootch is deployed
- AWS_THING_NAME_SPOOF : After creating a thing with the Admin CLI, add its "thing name" here

Navigate to {WOOTCH_PATH}/Cloud/aws_cdk/device_simulator

to send generated data on the backen end, you can run the following command:

```bash
$  npm run sendActivity
```

It will send sensor data online and if a user is paired to the device, a notification

To send a pairing request (to pair a user with the device simulator deviceId), run the following command:

```bash
$  npm run sendPairing
```

Your terminal will output a pairing secret, that you should keep to use it with the USER CLI.

## REAL DEVICE

Remark: Simply plugging the battery will not enable the device, a USB voltage (5V) through the USB-C connector will enable the system. in short, to start the system, place the battery (optional), and plug the USB-C connector. After that, if the battery is present, the USB-C can be unplugged. This is a common pattern electronic device whose battery not to be removed (easily). (It comes from the power supply selection IC behavior)

From now, we assume that the device has been programmed with valid credentials (certificate, private key and device ID).

The usage is fairly simple:

1. Press the reset button if the device is not on the "start page"
2. Press "start"
3. enter your wifi credentials
4. click on "pair" to generate credentials
5. Use the USER CLI to pair to the device
6. Change the device security level (move the slider to the right)
7. Shake the device
8. Check your emails
9. That's it

<br>

---

---

---

<br>

# HOW TO DEVELOP/MAINTAIN WOOTCH

## Update the Lambda functions

To update the express lambda functions

1. write/update some code in the folder {WOOTCH_PATH}/Cloud/aws_cdk/lambda/express
2. open a terminal and navigate to {WOOTCH_PATH}/Cloud/aws_cdk/build_tools
3. To update and re-deploy the express lambda function : run the following command

```bash
$  npm run expressUpdate
```

Same thing with iot lambda function.

There is a script to output the log of each lambda function with:

```bash
# print the log coming out of the iot lambda function (tail mode)
$  npm run iotLogTail
```

However using cloudwatch directly is much more convenient.

## Update CDK backend/services

To update the cloud severless backend (CDK)

```bash
$  npm run devSynth
$  npm run devDiff
$  npm run devDestroy
```
