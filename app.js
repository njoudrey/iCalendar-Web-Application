'use strict'

const mysql = require('mysql');

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

var usernamestore = '';
var passwordstore = '';

app.use(fileUpload());
app.use(express.json());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ********************
//char* createCalToJSON(char *fileName)
//char* createEventListToJSON(char *fileName)
//char* createAlarmListToJSON(char *fileName, int evtNum)
////char* createPropListToJSON(char *fileName, int evtNum)
//void createCalFile(char *fileName, char *calInfo, char *event);
//void addEventToCal(char *fileName, char*event)
let CalLib = ffi.Library('./libcal', {
  'createCalToJSON': [ 'string', ['string'] ],		//return type first, argument list second
  'createEventListToJSON': [ 'string', [ 'string' ] ],	//int return, int argument
  'createEventListToJSON2': [ 'string', [ 'string' ] ],
  'createPropListToJSON': ['string', ['string', 'int']],
  'createCalFile': ['void', ['string', 'string', 'string']],
  'addEventToCal': ['void', ['string', 'string']],
  'createAlarmListToJSON': ['string', ['string', 'int']],
});

//Sample endpoint
app.get('/filesinupload', function(req , res){
  let directory = "uploads";
  let dirBuf = Buffer.from(directory);
  let files = fs.readdirSync(directory);

  res.send({
      files
  });
});

app.get('/geteventlist', function(req , res){
  let list = CalLib.createEventListToJSON('./uploads/'+req.query.name);
  res.send({
      list
  });
});

app.get('/filesinfo', function(req , res){
  let directory = "uploads";
  let dirBuf = Buffer.from(directory);
  let files = fs.readdirSync(directory);
  var info = new Array(files.length);
  for(var i = 0; i < files.length; i++)
  {
    var obj = new Object();
    obj.name = files[i];
    obj.info = CalLib.createCalToJSON('./uploads/'+files[i]);
    obj.events = CalLib.createEventListToJSON2('./uploads/'+files[i]);
    info[i] = obj;
  }
  //let fileInfo = CalLib.createCalToJSON('./uploads/testCalSimpleUTC.ics');
  res.send({
    //fileInfo
    info
  });
});

app.get('/alarmlist', function(req , res){
  var alarms = CalLib.createAlarmListToJSON('./uploads/'+req.query.file, req.query.numEvent);
  //let fileInfo = CalLib.createCalToJSON('./uploads/testCalSimpleUTC.ics');
  res.send({
    alarms
  });
});

//char* createPropListToJSON(char *fileName, int evtNum)
app.get('/proplist', function(req , res){
  var props = CalLib.createPropListToJSON('./uploads/'+req.query.file, req.query.numEvent);
  //let fileInfo = CalLib.createCalToJSON('./uploads/testCalSimpleUTC.ics');
  res.send({
    props
  });
});

app.get('/createcal', function(req , res){
  CalLib.createCalFile(req.query.fileName, req.query.calInfo, req.query.eventInfo);
  res.send({
    status:"success"
  })
});

app.get('/addevent', function(req , res){
  CalLib.addEventToCal(req.query.fileName, req.query.eventInfo);
  res.send({
    status:"success"
  })
});

const mysqlConn = (user, pass, db) => {
  const conn = mysql.createConnection({
    host: 'dursley.socs.uoguelph.ca',
    user: user,
    password: pass,
    database: db
  });
  return conn;
}
app.post('/login', function(req, res){
  var {user, pass, db} = req.body;

  const connection = mysqlConn(user, pass, db);
// const connection = mysql.createConnection({
//     host: 'dursley.socs.uoguelph.ca',
//     user: req.query.user,
//     password: req.query.pass,
//     database: req.query.db
//   });
  connection.connect(function(err){
    if(err)
    {
      return res.status(400).send({
        message: 'This is an error!'
      });
    }
    else {
      res.send({message:"Success!"});
      usernamestore = req.body.user;
      passwordstore = req.body.pass;
      connection.query("create table if not exists file (cal_id int auto_increment primary key,  file_name varchar(60) not null,  version int not null, prod_id varchar(256) not null)", function (err, rows, fields) {
        if (err) console.log("Something went wrong. "+err);
      });
      connection.query("create table if not exists event (event_id int auto_increment primary key, summary varchar(1024), start_time datetime not null, location varchar(60), organizer varchar(256), cal_file int not null, foreign key(cal_file) references file(cal_id) on delete cascade)", function (err, rows, fields) {
        if (err) console.log("Something went wrong. "+err);
      });
      connection.query("create table if not exists alarm (alarm_id int auto_increment primary key, action varchar(256) not null, `trigger` varchar(256) not null, event int not null, foreign key(event) references event(event_id) on delete cascade)", function (err, rows, fields) {
        if (err) console.log("Something went wrong. "+err);
      });
    }
  });
});

app.post('/updatedb', function(req, res){

  const connection = mysqlConn(usernamestore, passwordstore, usernamestore);
  connection.connect(function(err){
    if(err){
      console.log("connection error while trying to save file info");
    }
    else{

      for(var i = 0; i < req.body.length; i++)
      {
        let name = req.body[i].name;
        let ver = JSON.parse(req.body[i].info).version;
        let id = JSON.parse(req.body[i].info).prodID;
        let events = JSON.parse(req.body[i].events);
        connection.query("SELECT * FROM file WHERE file_name = \'" + req.body[i].name+"\'", function(err, row){
          if (err){
             console.log("Something went wrong. "+err);
             return;
           }
           if(row && row.length)
           {
           }
           else{

             let sql = "INSERT INTO file (file_name, version, prod_id) VALUES (\'" +name+ "\', "+ver+", \'" +id+ "\')";
             connection.query(sql, function (err, result) {
               if (err) console.log("Something went wrong. "+err);

               for(let j = 0; j < events.length; j++)
               {
                 let sum = events[j].summary;
                 if(sum.length == 0)
                 {
                   sum = 'NULL';
                 }
                 let org = events[j].organizer;
                 if(org.length == 0)
                 {
                   org = 'NULL';
                 }
                 let loc = events[j].location;
                 if(loc.length == 0)
                 {
                   loc = 'NULL';
                 }
                 let alarms = events[j].alarms;
                 let dt = events[j].startDT;
                 let formatteddt = '\''+ dt.date[0]+dt.date[1]+dt.date[2]+dt.date[3]+'-'+dt.date[4]+dt.date[5]+'-'+dt.date[6]+dt.date[7]+' '+dt.time[0]+dt.time[1]+':'+dt.time[2]+dt.time[3]+':'+dt.time[4]+dt.time[5]+'\'';
                 let evsql = "INSERT INTO event (start_time, cal_file, summary, organizer, location) VALUES ("+formatteddt+" , (SELECT cal_id FROM file WHERE file_name = \'"+name+"\'), \'"+sum+"\', \'"+org+"\', \'"+loc+"\')";
                 connection.query(evsql, function (err, result) {
                   if (err) console.log("Something went wrong. "+err);
                   for(let z = 0; z < alarms.length; z++)
                   {
                     let alsql = "INSERT INTO alarm (action, `trigger`, event) VALUES (\'"+alarms[z].action+"\', \'"+alarms[z].trigger+"\', (SELECT event_id FROM event WHERE cal_file = (SELECT cal_id FROM file WHERE file_name = \'"+name+"\') LIMIT "+j+",1))";
                     connection.query(alsql, function (err, result) {
                       if (err) console.log("Something went wrong. "+err);

                     });
                   }
                 });
               }
             });
           }
        });
      }
    }


  });
  res.send();
});

app.get('/clear', function(req, res){
  const connection = mysqlConn(usernamestore, passwordstore, usernamestore);
  connection.connect(function(err){
    if (err) console.log("Something went wrong. "+err);

    let sql = "DELETE FROM alarm";
    connection.query(sql, function (err, result) {
      if (err) console.log("Something went wrong. "+err);
      let sql = "DELETE FROM event";
      connection.query(sql, function (err, result) {
        if (err) console.log("Something went wrong. "+err);
        let sql = "DELETE FROM file";
        connection.query(sql, function (err, result) {
          if (err) console.log("Something went wrong. "+err);
        });
      });
    });
  });

  res.send();
});



app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
