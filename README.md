# BarcodeReader
BarcodeReader capture the decimal from 48 to 122 in ASCII table standard sent from the scanner and convert it to characters. The string is then sent to the automatic optical inspection machine. At this moment, the app has only draft form of code.

How it works.
- The scanner before AOI reads the barcode from the PCB
- The app capture the stream in the COM 10
- Dec code is converting to char and written to file .txt 
- Other app "RnW" waiting until PCB enter to the AOI then take the barcode from txt file and deliver it to the GUI AOI.
