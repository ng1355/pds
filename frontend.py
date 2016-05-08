#!/usr/bin/python
from flask import Flask, render_template, request, url_for, session, redirect, abort
from socket import *

app = Flask(__name__)


def comm(msg, len = 1000):
	sock = socket(AF_INET, SOCK_STREAM)
	sock.connect(("localhost", 1234))
	sock.sendall(msg)
	data = sock.recv(len) 
	sock.shutdown(1)
	sock.close()
	#print "message:\n" + msg + " \n--\ndata: " + data + '\n--\n'
	return data

def validLogin(uname, pword):
	data = comm('validLogin\n' + uname + '\n' + pword)
	return data

def inSession():
	if "uname" in session:
		return comm("inSession\n" + session.get("uname", None))
	return '0'

def logMeIn(uname):
	comm("logMeIn\n" + uname + "\n")
	session["uname"] = uname
	return redirect(url_for("profile", uname = uname))

@app.route("/", methods=['POST', 'GET'])
def root(invalid = False):
	if inSession() == '1':
		return redirect(url_for("profile", uname = session.get("uname", None)))
	if request.method == 'POST':
		if validLogin(request.form["uname"], request.form["pword"]) == '1':
			return logMeIn(request.form["uname"])
		else:
			return render_template("root.html", invalid = True)
	return render_template("root.html")

@app.route("/register", methods=['POST', 'GET'])
def register(loggedIn = False, taken = False, invalid = False):
	if inSession() == '1':
		return render_template("register.html", loggedIn = True, uname = session.get("uname", None))
	if request.method == 'POST':
		uname = request.form["uname"]
		if comm("isRegistered\n" + uname) == '1':
			return render_template("register.html", taken = True)
		elif comm("register\n" + uname + '\n' + request.form["pword"]) == "0":
			return render_template("register.html", invalid = True)
		else:
			pass
		return redirect(url_for("root"))
	return render_template("register.html")

@app.route("/surveys/", methods=['POST', 'GET'])
def surveyList(empty = False):
	listOfSurveyNames = comm("surveyNames\n").split("\t")
	if(listOfSurveyNames[0] == ""):
		return render_template("surveyList.html", empty = True)
	else:
		listOfSurveyNames.pop()
	return render_template("surveyList.html", surveyList = listOfSurveyNames)

@app.route("/surveys/<name>", methods=['POST', 'GET'])
def survey(name = None, invalid = False):
	tmp = comm("getSurvey\n" + name).split("\n")
	usersVoted = comm("whoVoted\n" + name).split('\n')
	if tmp[0] == '0':
		abort(404)
	if usersVoted[0] == "":
		usersVoted = {"None yet!"}
	else:
		usersVoted.pop()
	if request.method == 'POST':
		if request.form.get("option1", "error") == tmp[2]:
			if comm("vote\n" + '1\n' + session.get("uname", "Anon") + '\n' + tmp[0]) == '0':
				return render_template("survey.html", invalid = True, name=tmp[0], question = tmp[1], option1=tmp[2], option2=tmp[3], owner=tmp[4], option1Percent=tmp[5], option2Percent=tmp[6], numOfVotes=tmp[7], usersVoted = usersVoted)
		elif request.form.get("option2", "error") == tmp[3]:
			if comm("vote\n" + '2\n' + session.get("uname", "Anon") + '\n' + tmp[0]) == '0':
				return render_template("survey.html", invalid = True, name=tmp[0], question = tmp[1], option1=tmp[2], option2=tmp[3], owner=tmp[4], option1Percent=tmp[5], option2Percent=tmp[6], numOfVotes=tmp[7], usersVoted = usersVoted)
		tmp = comm("getSurvey\n" + name).split("\n")
		usersVoted = comm("whoVoted\n" + name).split('\n')
		if usersVoted[0] == "":
			usersVoted = {"None yet!"}
		else:
			usersVoted.pop()
	return render_template("survey.html", name=tmp[0], question = tmp[1], option1=tmp[2], option2=tmp[3], owner=tmp[4], option1Percent=tmp[5], option2Percent=tmp[6], numOfVotes=tmp[7], usersVoted = usersVoted)

@app.route("/createSurvey", methods=['POST', 'GET'])
def createSurvey():
	if request.method == 'POST':
		#listOfSurveys[request.form["name"]] = surveyData(session.get("uname", "Anonymous"), request.form["name"], request.form["question"], request.form["option1"], request.form["option2"])
		comm("createSurvey\n" + session.get("uname", "Anonymous") + '\n' + request.form["name"] + '\t' + request.form["question"] + '\t' + request.form["option1"] + '\t' + request.form["option2"])
		return redirect(url_for("survey", name=request.form["name"]))
	return render_template("createSurvey.html")

#@app.route("/profile/", methods=['POST', 'GET'])
@app.route("/profile/<uname>", methods=['POST', 'GET'])
def profile(uname = None):
	comm("isRegistered\n" + uname) #stops if from misbehaving for some reason
	if comm("isRegistered\n" + uname) == '0':
		abort(404)
	votedOn = comm("votedOn\n" + uname).split('\n')
	print "voted on: "
	print(votedOn)
	if votedOn[0] == "":
	 	votedOn = {"None yet!"}
	else:
		votedOn.pop()
	return render_template("profile.html", uname=uname, votedOn = votedOn)

@app.route("/logout", methods=['POST', 'GET'])
def logout():
	if inSession() == '0':
		abort(401)
	comm("logout\n" + session["uname"])
	session.pop("uname", None)
	return redirect(url_for("root"))

@app.route("/deleteAccount", methods=['POST', 'GET'])
def deleteAccount():
	if inSession() == '0':
		abort(401)
	comm("deleteAccount\n" + session["uname"])
	session.pop("uname", None)
	return redirect(url_for("root"))

app.secret_key = "lol"
app.run("localhost", 13000, debug=True)
