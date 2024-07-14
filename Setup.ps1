# Basic setup script to install all the requirements for the ESP8266 Wearable Development Board firmware.
# Project: ESP8266 Wearable Development Board firmware.
# Filename: Setup.ps1
# Author: John Wolfe
# Date: 13/07/2024

#   Copyright 2024 WolfNet Computing©
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

# Here we check whether the host Powershell is "Windows Powershell" or "Powershell 7" and act accordingly.
If ($PSVersionTable.PSVersion.Major -lt 7) {
	# We are running on "Windows Powershell" and we need to switch to "Powershell 7"
	try
	{
		pwsh $MyInvocation.MyCommand.Name
	}
	catch [System.Management.Automation.CommandNotFoundException]
	{
		Write-Host "Installing Powershell 7..."
		& winget install --id Microsoft.Powershell --source winget
		$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User") 
		pwsh $MyInvocation.MyCommand.Name
	}
}
Else
{
	# Setting $PYTHON_EXE within these if statements allows for OS specific differences in the naming of the Python3 executable.
	# The Else statement allows the script to detect an unsupported OS and take appropriate actions.
	If ($IsWindows) {
		$PYTHON_EXE = "python"
		If (Test-Path -Path "${env:LOCALAPPDATA}\Microsoft\WindowsApps\py.exe")
		{
			Remove-Item "${env:LOCALAPPDATA}\Microsoft\WindowsApps\py.exe"
		}
		If (Test-Path -Path "${env:LOCALAPPDATA}\Microsoft\WindowsApps\python.exe")
		{
			Remove-Item "${env:LOCALAPPDATA}\Microsoft\WindowsApps\python.exe"
		}
		If (Test-Path -Path "${env:LOCALAPPDATA}\Microsoft\WindowsApps\python3.exe")
		{
			Remove-Item "${env:LOCALAPPDATA}\Microsoft\WindowsApps\python3.exe"
		}
		$ARDUINO_LIBS = "C:\Users\${env:USERNAME}\Documents\Arduino\libraries\"
	}
	ElseIf ($IsLinux -Or $IsMacOS) {
		$PYTHON_EXE = "python3"
		If ($IsLinux) {
			$ARDUINO_LIBS = "/home/${env:USER}/Arduino/libraries/"
		}
		Else {
			$ARDUINO_LIBS = "/Users/${env:USER}/Documents/Arduino/libraries/"
		}
	}
	Else {
		# If we ever get here then this was attempted to be installed on an OS that we don't support.
		Write-Host 'Unsupported OS! Setup will now send a "friend" to your location to "assist" with your unfortunate situation.'
		Write-Host 'Only joking about sending a friend, however, your OS is still unsupported and setup will now quit.'
		Exit 1
	}

	try
	{
		# Upgrade the PIP package first.
		Write-Host "Attempting to update PIP..."
		& $PYTHON_EXE '-m' 'pip' 'install' '--upgrade' 'pip' | Out-Null
	}
	catch [System.Management.Automation.CommandNotFoundException]
	{
		Write-Host "Python 3 is NOT installed..."
		If ($IsWindows) {
			New-Item -name "./Temp" -path . -itemType Directory | Out-Null
			Write-Host "Downloading Python 3..."
			Invoke-WebRequest -Uri "https://www.python.org/ftp/python/3.12.4/python-3.12.4.exe" -OutFile "./Temp/Python3_Setup.exe" | Out-Null
			Write-Host "Installing Python 3..."
			& ./Temp/Python3_Setup.exe '/passive' 'AppendPath=1' 'SimpleInstall=1' 'SimpleInstallDescription="Single User installation and append location to PATH variable."' | Out-Null
			Remove-Item -Path "./Temp" -Force -Recurse
		}
		Write-Host "Reattempting to update PIP..."
		& $PYTHON_EXE '-m' 'pip' 'install' '--upgrade' 'pip' | Out-Null
	}

	# Now attempt to install the esptool package.
	Write-Host "Installing esptool..."
	& $PYTHON_EXE '-m' 'pip' 'install' '--upgrade' 'esptool' | Out-Null

	$downloads = @("https://github.com/khoih-prog/ESP_MultiResetDetector/archive/refs/heads/main.zip","https://github.com/pangodream/ESP2SOTA/archive/refs/heads/main.zip","https://github.com/gadaman-rm/ESP_MultiWiFi/archive/refs/heads/main.zip","https://github.com/LennartHennigs/ESPTelnet/archive/refs/heads/main.zip","https://github.com/Tost69/ConfigStorage/archive/refs/heads/main.zip","https://github.com/bblanchon/ArduinoJson/archive/refs/heads/7.x.zip","https://github.com/yurilopes/SPIFFSIniFile/archive/refs/heads/master.zip","https://github.com/radekrudak/BetterOTA/archive/refs/heads/master.zip","https://github.com/usini/usini_discord_webhook/archive/refs/heads/main.zip","https://github.com/ofekp/TinyUPnP/archive/refs/heads/master.zip","https://github.com/gemi254/ConfigAssist-ESP32-ESP8266/archive/refs/heads/main.zip","https://github.com/ThingPulse/esp8266-oled-ssd1306/archive/refs/heads/master.zip","https://github.com/vshymanskyy/Preferences/archive/refs/heads/main.zip","https://github.com/tobozo/YAMLDuino/archive/refs/heads/main.zip","https://github.com/khoih-prog/ESP_WiFiManager/archive/refs/heads/master.zip","https://github.com/FastLED/FastLED/archive/refs/heads/master.zip")
	foreach ($download in $downloads) {
		Write-Host "Downloading and extracting file: ${download}"
		Invoke-WebRequest -Uri $download -OutFile ".\download.zip"
		Expand-Archive -Path ".\download.zip" -DestinationPath $ARDUINO_LIBS -Force
	}
	Remove-Item -Path ".\download.zip"
	
}