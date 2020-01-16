var curFile = "";
var loggedIn = 0;
var numFiles = 0;
var validFiles = [];
var saveBtn = 0;

$(document).ready(function() {
    //update/load file table and file selector
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/filesinfo',
        success: function (data) {
            console.log(data);
            if(data.info.length > 0)
            {
              $('#FileLog').html(generate_file_table(data.info));
            }
        },
        fail: function(error) {
            console.log("Error updating file table and calendar selctor tab " + data);
            statmes("Error getting/updating file table and selector tabs on page load");
        }
    });


    $('#loginbtn').click(function(e){
      var username = document.getElementById("userEntryBox").value;
      var password = document.getElementById("passEntryBox").value;
      var data = document.getElementById("dataEntryBox").value;
      console.log("Login Attempt " + username +" " + password);
      statmes('Attempting to login');
      $.ajax({
        type: 'post',
        dataType: 'json',
        data: JSON.stringify({user: username, pass: password, db: data}),
        contentType: 'application/json',
        url: '/login',
        success: function (data){
          loggedIn = 1;
          var ele = document.getElementById("loginstat");
          $("#loginstat").html("");
          ele.setAttribute("class", "alert alert-success");

          var a = document.createElement("a");
          a.setAttribute("href", "#");
          a.setAttribute("class", "close");
          a.setAttribute("data-dismiss", "alert");
          a.setAttribute("aria-label", "close");
          var text = document.createTextNode("X");
          a.appendChild(text);
          ele.appendChild(a);

          var text2 = document.createTextNode("Succesfully logged in as " + username + "!");
          ele.appendChild(text2);
          $("#login").html("");
          generateDBArea();
          //<a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>

          console.log("Success");
          statmes('Login successful');
        },
        error: function(err){
          $('#loginstat').html("Login unsuccessful");
          console.log("Fail");
          statmes('Login unsuccessful');
        }
      });
    });


    $('#eventaddtocal').click(function(e){
      var event1 = document.getElementById("selectcaltoadd");
      var event2 = document.getElementById("selectedevent");
      console.log("adding " + event2.value + " to " + event1.value);
      if(event1.value != 0 && event2.value != 0)
      {
      $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          data: {fileName: './uploads/'+event1.value, eventInfo:event2.value},
          url: '/addevent',   //The server endpoint we are connecting to
      });
      $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/filesinfo',   //The server endpoint we are connecting to
          success: function (data) {
              /*  Do something with returned object
                  Note that what we get is an object, not a string,
                  so we do not need to parse it on the server.
                  JavaScript really does handle JSONs seamlessly
              */

              $('#FileLog').html(generate_file_table(data.info));
              //We write the object to the console to show that the request was successful
              console.log(data);
          },
          fail: function(error) {
              // Non-200 return, do something with error
              console.log(data);
          }
      });
      statmes("Event added to Calendar Succesfully");
      }
      else {
        {
          statmes("Error adding an event to a calendar, both a calendar and an event must be selected");
        }
      }
    });
    //Event listener for the submit file button
    $('#subme').click(function(e){
        e.preventDefault();
        myFunc();
        console.log('Callback from submit button');
    });

    $('#subcal').click(function(e){
      var form = document.getElementById('calform');
      var pid = form.pidEntryBox.value;
      var ver = parseInt(form.verEntryBox.value);
      var calName = form.fnEntryBox.value;
      var evt = form.selectevt.value;
      var obj = new Object();
      obj.version = ver;
      obj.prodID = pid;
      console.log("event recieved " + JSON.stringify(obj) + evt);
      e.preventDefault();
      //app.get('/createcal', function(req , res){
      //  CalLib.createCalFile(req.query.fileName, req.query.calInfo, req.query.eventInfo);
      //});
      if(calName.length == 0 || pid.length == 0 || ver.length == 0 || isNaN(ver) || evt == 0 || calName.includes('.ics') == false)
      {
        statmes("Error creating calendar check value field and ensure all values are correct and an event is selected");
        return;
      }
      $.ajax({
          type: 'get',
          dataType: 'json',
          data: {fileName:'./uploads/'+calName, calInfo:JSON.stringify(obj), eventInfo:evt},
          url: '/createcal',
          success: function (data) {
              console.log("success");
          },
          fail: function(error) {
              console.log("Error updating file table and calendar selctor tab " + data);
          }
      });
      $.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/filesinfo',   //The server endpoint we are connecting to
          success: function (data) {

              $('#FileLog').html(generate_file_table(data.info));
              //We write the object to the console to show that the request was successful
              console.log(data);
          },
          fail: function(error) {
              // Non-200 return, do something with error
              console.log(data);
          }
      });
      statmes("Calendar created Succesfully");
    });
    //event listener for selector
    $('#subevt').click(function(e){
      var element = document.getElementById('evtform');
      var evtName = element.nameEntryBox.value;
      var uid = element.uidEntryBox.value;
      var createD = element.creationDEntryBox.value;
      var createT = element.creationTEntryBox.value;
      var startD = element.startDEntryBox.value;
      var startT = element.startTEntryBox.value;
      var sum = element.sumEntryBox.value;
      if(evtName.length == 0)
      {
        statmes("Error creating event please enter a valid Event Name ie. length greater than 0");
        return;
      }
      if(uid.length == 0)
      {
        statmes("Error creating event please enter a valid uid");
        return;
      }
      if(createD.length != 8 || isNaN(createD) || startD.length != 8 || isNaN(startD) || createT.length != 6 || isNaN(createT) || startT.length != 6 || isNaN(startT))
      {
        statmes("Error creating event, invalid length of time or date or value entered is not an integer");
        return;
      }
      //date, time, isUTC
      var obj = new Object();
      var startDT = new Object();
      obj.uid = uid;
      startDT.date = startD;
      if(element.startUTC.value == 1){
        startDT.isUTC = true;
      }
      else {
        startDT.isUTC = false;
      }
      startDT.time = startT;
      obj.startDT = startDT;

      var creationDT = new Object();
      creationDT.date = createD;

      if(element.createUTC.value == 1){
        creationDT.isUTC = true;
      }
      else {
        creationDT.isUTC = false;
      }
      creationDT.time = createT;
      obj.creationDT = creationDT;
      if(sum.length > 0)
      {
        obj.summary = sum;
      }
      console.log('adding string:' + JSON.stringify(obj));

      var eventTab = document.getElementById("selectevt");
      var newopt = document.createElement("option");
      var opttext = document.createTextNode(evtName);
      newopt.appendChild(opttext);
      newopt.setAttribute("value", JSON.stringify(obj));
      eventTab.appendChild(newopt);

      var evtappend = document.getElementById("selectedevent");
      var newopt1 = document.createElement("option");
      var opttext1 = document.createTextNode(evtName);
      newopt1.appendChild(opttext1);
      newopt1.setAttribute("value", JSON.stringify(obj));
      evtappend.appendChild(newopt1);

      e.preventDefault();
      statmes("Event created Succesfully");
    });

    $('#list').change(function(e){
        var element = document.getElementById('select2');
        let x = element.value;
        curFile = element.value;
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            data: {name: x},
            url: '/geteventlist',
            success: function (data) {
                $('#alarmtable').html("");
                $('#optionproptable').html("");
                generate_cal_table(data.list);
            },
            fail: function(error) {
                console.log(data);
            }
        });
        console.log(element.value + ' selected');
    });
    $('#showalarms').change(function(e){
        var element = document.getElementById('alarmselector');
        let x = element.value;
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            data: {numEvent: x, file: curFile},
            url: '/alarmlist',
            success: function (data) {
                generate_alarm_table(data.alarms);
            },
            fail: function(error) {
                console.log(data);
            }
        });
        console.log(element.value + ' selected');
    });

    $('#optionprops').change(function(e){
        var element = document.getElementById('propselector');
        let x = element.value;
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            data: {numEvent: x, file: curFile},
            url: '/proplist',
            success: function (data) {
                generate_prop_table(data.props);
            },
            fail: function(error) {
                console.log(data);
            }
        });
        console.log(element.value + ' selected');
    });
});

function generate_alarm_table(jString)
{
  var obj = JSON.parse(jString);
  $('#alarmtable').html("");
  if(jString == null || obj.length == 0)
  {
    return;
  }
  var table = document.createElement("table");
  var tableBody = document.createElement("tbody");
  var row = document.createElement("tr");

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Alarm Number');
  cell.appendChild(cellText);
  row.appendChild(cell);

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Action');
  cell.appendChild(cellText);
  row.appendChild(cell);

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Trigger');
  cell.appendChild(cellText);
  row.appendChild(cell);

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Number of Optional Properties');
  cell.appendChild(cellText);
  row.appendChild(cell);

  tableBody.appendChild(row);
  for(var i = 0; i < obj.length; i++)
  {
    var row = document.createElement("tr");

    var cell = document.createElement("td");
    var cellText = document.createTextNode(i+1);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].action);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].trigger);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].numProps);
    cell.appendChild(cellText);
    row.appendChild(cell);
    tableBody.appendChild(row);
  }
  table.appendChild(tableBody);
  table.setAttribute("class", "table table-bordered");
  $('#alarmtable').html(table);
}

function generate_prop_table(jString)
{
  var obj = JSON.parse(jString);
  $('#optionproptable').html("");
  if(jString == null || obj.length == 0 || (obj.length == 1 && obj[0].name == 'SUMMARY'))
  {
    return;
  }
  var table = document.createElement("table");
  var tableBody = document.createElement("tbody");
  var row = document.createElement("tr");

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Property Name');
  cell.appendChild(cellText);
  row.appendChild(cell);

  var cell = document.createElement("td");
  var cellText = document.createTextNode('Property Description');
  cell.appendChild(cellText);
  row.appendChild(cell);

  tableBody.appendChild(row);
  for(var i = 0; i < obj.length; i++)
  {
    if(obj[i].name != 'SUMMARY')
    {
    var row = document.createElement("tr");

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].name);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].descr);
    cell.appendChild(cellText);
    row.appendChild(cell);
    tableBody.appendChild(row);
    }
  }
  table.appendChild(tableBody);
  table.setAttribute("class", "table table-bordered");
  $('#optionproptable').html(table);
}

function generate_file_table(jString){
  var table = document.createElement("table");
  var tableBody = document.createElement("tbody");

  var titles = ["File name (click to download)", "Version", "Product ID", "Number of events", "Number of properties"];

  var validF = [];

  var valid = 0;
  for(var i = 0; i < jString.length; i++)
  {
    var obj = JSON.parse(jString[i].info);
    if(obj != null)
    {
      validF[valid] = jString[i];
      valid = valid + 1;
    }
  }
  validFiles = validF;
  if(valid != 0)
  {
  var row = document.createElement("tr");
  for(var i = 0; i < 5; i++)
  {
    var cell = document.createElement("td");
    var cellText = document.createTextNode(titles[i]);
    cell.appendChild(cellText);
    row.appendChild(cell);
  }
  tableBody.appendChild(row);
  }

  //tab setup
  var dropdown = document.createElement("select");
  var dropdown2 = document.createElement("select");

  var a = document.createElement("option");
  var aText = document.createTextNode("No File Selected");
  a.setAttribute("value", 0);
  a.appendChild(aText);
  dropdown.setAttribute("id", "select2");

  dropdown.appendChild(a);
  //
  var b = document.createElement("option");
  var bText = document.createTextNode("No File Selected");
  b.setAttribute("value", 0);
  b.appendChild(bText);
  dropdown2.setAttribute("id", "selectcaltoadd");

  dropdown2.appendChild(b);
  var count = 0;
  for(var i = 0; i < jString.length; i++)
  {
    var obj = JSON.parse(jString[i].info);
    if(obj != null)
    {
    count = count + 1;
    var row = document.createElement("tr");

    var cell = document.createElement("td");
    var a = document.createElement("a");
    var cellLinkd = document.createTextNode(jString[i].name);
    a.setAttribute("href", "/uploads/"+jString[i].name);
    a.appendChild(cellLinkd);

    cell.appendChild(a);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj.version);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj.prodID);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj.numEvents);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj.numProps);
    cell.appendChild(cellText);
    row.appendChild(cell);
    tableBody.appendChild(row);

    //do the addtabs
    var a = document.createElement("option");
    var aText = document.createTextNode(jString[i].name);
    a.appendChild(aText);
    a.setAttribute("value", jString[i].name);
    dropdown.appendChild(a);

    var b = document.createElement("option");
    var bText = document.createTextNode(jString[i].name);
    b.appendChild(bText);
    b.setAttribute("value", jString[i].name);
    dropdown2.appendChild(b);
    //
    }
  }
  numFiles = count;
  $('#list').html(dropdown);
  $('#list2').html("");
  var list2 = document.getElementById("list2");
  var label = document.createElement("label");
  label.setAttribute("for", "selectcaltoadd");
  var labelTxt = document.createTextNode("Calendar: ");
  label.appendChild(labelTxt);
  list2.appendChild(label);
  list2.appendChild(dropdown2);
  table.appendChild(tableBody);
  table.setAttribute("class", "table table-bordered");
  generateDBArea();
  return table;
}

function generate_cal_table(jString)
{
  statmes("Creating/updating event table for File View Panel");
  $('#showalarms').html("");
  $('#optionprops').html("");
  console.log('clearing alarm table and optional property tables');
  if(jString == null)
  {
    $('#calviewtable').html("");
    return;
  }

  //show alarm selector
  //<label for="favorite-animal">Favorite Animal</label>
  var label = document.createElement("label");
  var labelTxt = document.createTextNode("Show Alarms for Event:");
  label.appendChild(labelTxt);
  label.setAttribute("for", "alarmselector");
  var divName = document.getElementById("showalarms");
  divName.appendChild(label);
  var select = document.createElement("select");
  select.setAttribute("id", "alarmselector");
  var noalarm = document.createElement("option");
  noalarm.setAttribute("value", "0");
  var noalarmtxt = document.createTextNode("---");
  noalarm.appendChild(noalarmtxt);
  select.appendChild(noalarm);
  divName.appendChild(select);


  //optional prop slector
  var label = document.createElement("label");
  var labelTxt = document.createTextNode("Show Optional properties for Event:");
  label.appendChild(labelTxt);
  label.setAttribute("for", "propselector");
  var divName = document.getElementById("optionprops");
  divName.appendChild(label);
  var option = document.createElement("select");
  option.setAttribute("id", "propselector");
  var nooption = document.createElement("option");
  nooption.setAttribute("value", "0");
  var nooptiontxt = document.createTextNode("---");
  nooption.appendChild(nooptiontxt);
  option.appendChild(nooption);
  divName.appendChild(option);

  var obj = JSON.parse(jString);

  var table = document.createElement("table");
  var tableBody = document.createElement("tbody");

  var titles = ["Event No", "Start date", "Start time", "Summary", "Props", "Alarms"];

  var row = document.createElement("tr");
  for(var i = 0; i < 6; i++)
  {
    var cell = document.createElement("td");
    var cellText = document.createTextNode(titles[i]);
    cell.appendChild(cellText);
    row.appendChild(cell);
  }
  tableBody.appendChild(row);

  for(var i = 0; i < obj.length; i++)
  {
    var row = document.createElement("tr");

    var cell = document.createElement("td");
    var cellText = document.createTextNode(i+1);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var formattedDate = obj[i].startDT.date[0] + obj[i].startDT.date[1] + obj[i].startDT.date[2] + obj[i].startDT.date[3] + '/' + obj[i].startDT.date[4] +obj[i].startDT.date[5] + '/' + obj[i].startDT.date[6] + obj[i].startDT.date[7];
    var cellText = document.createTextNode(formattedDate);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    if(obj[i].startDT.isUTC){
      var timeF = obj[i].startDT.time[0] + obj[i].startDT.time[1] + ':' + obj[i].startDT.time[2] + obj[i].startDT.time[3] + ':' + obj[i].startDT.time[4] + obj[i].startDT.time[5] + ' (UTC)';
    }
    else{
      var timeF = obj[i].startDT.time[0] + obj[i].startDT.time[1] + ':' + obj[i].startDT.time[2] + obj[i].startDT.time[3] + ':' + obj[i].startDT.time[4] + obj[i].startDT.time[5];
    }
    var cellText = document.createTextNode(timeF);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].summary);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].numProps);
    cell.appendChild(cellText);
    row.appendChild(cell);

    var cell = document.createElement("td");
    var cellText = document.createTextNode(obj[i].numAlarms);
    cell.appendChild(cellText);
    row.appendChild(cell);

    tableBody.appendChild(row);

    //add event to selectors
    //alarmselector propselector
    var alarmSelect = document.getElementById("alarmselector");
    var proSelect = document.getElementById("propselector");


    let val = i+1;
    var text = document.createTextNode(val);
    var opt = document.createElement("option");
    opt.appendChild(text);
    opt.setAttribute("value", val);
    alarmSelect.appendChild(opt);

    var text = document.createTextNode(val);
    var opt = document.createElement("option");
    opt.appendChild(text);
    opt.setAttribute("value", val);
    proSelect.appendChild(opt);
  }

  table.appendChild(tableBody);
  table.setAttribute("class", "table table-bordered");
  $('#calviewtable').html(table);

}

function myFunc() {
    var data = document.getElementById('fileData');
    var form = new FormData();
    form.append(data.name, data.files[0]);
    fetch('/upload', {
        method: "POST",
        body: form
    })
    .then((res) => {
        var msg = document.getElementById('msg');
        if(res.status === 200) {
          statmes("Succesfully uploaded file");
          console.log("Success uploading "+data);
          $.ajax({
              type: 'get',            //Request type
              dataType: 'json',       //Data type - we will use JSON for almost everything
              url: '/filesinfo',   //The server endpoint we are connecting to
              success: function (data) {
                  /*  Do something with returned object
                      Note that what we get is an object, not a string,
                      so we do not need to parse it on the server.
                      JavaScript really does handle JSONs seamlessly
                  */
                  $('#FileLog').html(generate_file_table(data.info));
                  //We write the object to the console to show that the request was successful
                  console.log("success getting updated file info " + data);
              },
              fail: function(error) {
                  // Non-200 return, do something with error
                  console.log("failure getting updated file info " + data);
              }
          });
        }
        else {
            //error
            statmes("Error uploading file");
            console.log("Error trying to upload " + data);
        }
    })

    .catch(err => console.log(err));
}

function clear_stat_reg()
{
  console.log("clearing table");
  $('#statinfo').html("");
}

function statmes(message)
{
  var listitem = document.createElement("div");
  var text = document.createTextNode(message);
  listitem.appendChild(text);
  var statpanel = document.getElementById("statinfo");
  statpanel.appendChild(listitem);
  console.log("adding stat to stat panel");

}

function clearMee()
{
  statmes('clearing files currently stored');
  $.ajax({
    type: 'get',            //Request type
    dataType: 'json',       //Data type - we will use JSON for almost everything
    url: '/clear',   //The server endpoint we are connecting to
  });
}

function generateDBArea()
{
  if(loggedIn == 1 && numFiles != 0 && saveBtn != 1)
  {
    saveBtn = 1;
    var db = document.getElementById("dbfunc");
    var btn = document.createElement("input");
    btn.setAttribute("id", "storefiles");
    btn.setAttribute("value", "Store Files");
    btn.setAttribute("type", "button");
    btn.setAttribute("class", "btn btn-secondary");
    btn.setAttribute("onclick", "store()");
    db.appendChild(btn);

    var br = document.createElement("br");
    db.appendChild(br);
    var br1 = document.createElement("br");
    db.appendChild(br1);
    var db1 = document.getElementById("dbfunc1");
    var btn1 = document.createElement("input");
    btn1.setAttribute("id", "clfiles");
    btn1.setAttribute("value", "Clear Tables");
    btn1.setAttribute("type", "button");
    btn1.setAttribute("class", "btn btn-secondary");
    btn1.setAttribute("onclick", "clearMee()");
    db1.appendChild(btn1);
  }
}

function store()
{
  console.log(validFiles);
  statmes('storing files on server');
  console.log(validFiles);
  $.ajax({
    type: 'POST',            //Request type
    dataType: 'application/json',       //Data type - we will use JSON for almost everything
    data: JSON.stringify(validFiles),
    contentType: 'application/json',
    url: '/updatedb'
  });
}
