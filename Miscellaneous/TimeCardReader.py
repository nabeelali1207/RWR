import csv
import glob
import tkinter as tk
import os

data = []
class timeData:
	def __init__(self, hours, minutes, seconds) -> None:
		self.hours = hours
		self.minutes = minutes
		self.seconds = seconds
	def getHours(self):
		return self.hours
	def getMinutes(self):
		return self.minutes
	def getSeconds(self):
		return self.seconds
	def printTime(self):
		print(self.hours + self.minutes + self.seconds)

def parseFile():
	global data
	# os.chdir("Miscellaneous/")
	csv_files = glob.glob("Miscellaneous/Time Card_*_export.csv")
	print(csv_files)
	for filename in csv_files:
		with open(filename) as csvfile:
			reader = csv.reader(csvfile)
			print(reader)
			for i, row in enumerate(reader):
				if i != 0:
					data.append(row)
	data = [[row[3], row[4], row[5]] for row in data if len(row) >= 6]

def calculateTimeDifference():
	global data
	myIN = data
	inHours = [((10 * int(row[2][0])) + int(row[2][1])) for row in data]
	inMinutes = [((10 * int(row[2][3])) + int(row[2][4])) for row in data]
	inSeconds = [((10 * int(row[2][6])) + int(row[2][7])) for row in data]
	for index, value in enumerate(inSeconds):
		myIN[index] = inHours[index] * 60 * 60 + inMinutes[index] * 60 + inSeconds[index]
	outHours = [((10 * int(row[2][0])) + int(row[2][1])) for row in data]
	outMinutes = [((10 * int(row[2][3])) + int(row[2][4])) for row in data]
	outSeconds = [((10 * int(row[2][6])) + int(row[2][7])) for row in data]

	print(myIN)


# def calculateTimeDifference():
# 	pass

parseFile()

print()
calculateTimeDifference()
print(data)

# for index, value in enumerate(data):
# 	hours = 





# main = tk.Tk()
# main.title('Hello')
# # button = tk.Button(main, text = 'Stop', width = 25, height = 15, command = main.destroy)
# # button.pack()
# main.mainloop()





# # print(os.listdir())
# os.chdir("Miscellaneous/")

# # csv_files = glob.glob("Miscellaneous/Time Card_*_export.csv")
# csv_files = glob.glob("Time Card_*_export.csv")
# print(csv_files)
# # Loop through each CSV file and read the data
# for filename in csv_files:
# 	print(filename)
# 	with open(filename) as csvfile:
# 		reader = csv.reader(csvfile)
# 		print(reader)
# 		for i, row in enumerate(reader):
# 			# Process the row
# 			if i != 0:
# 				print(row)