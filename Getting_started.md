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

```powershell
$ cd /destination/folder
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

At the moment, the certificate, key and ID are directly flashed into the device.

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

All design files are provided to build the boards.

We had a good experience with PCBway, but any PCB maker/assembler would do the job.

<br>

---

---

---

<br>

# HOW TO USE WOOTCH

## ADMIN CLI

## USER CLI

## DEVICE SIMULATOR

Fill the following secrets from {WOOTCH_PATH}/Cloud/aws_cdk/device_simulator/secrets/secrets.ts

- AWS_IOT_ENDPOINT : AWS iot-core endpoint (console -> iot-core -> settings -> endpoint )
- AWS_REGION : AWS region where Wootch is deployed
- AWS_THING_NAME_SPOOF : After creating a thing with the Admin CLI, add it "thing name" here

## REAL DEVICE

<br>

---

---

---

<br>

# HOW TO DEVELOP/MAINTAIN WOOTCH
