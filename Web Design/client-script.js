const url= "wss://wandering-temporal-foxtrot.glitch.me/client-main.html"
const connection = new WebSocket(url);

connection.onopen = () => {
    connection.send(JSON.stringify({"type": "client-connected"}));
}

var xhr = new XMLHttpRequest();
xhr.open("GET", "/get-current-playlist");
xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

xhr.onloadend = function(e) {
    console.log(JSON.parse(xhr.responseText));
    var message = JSON.parse(xhr.responseText);
    for(var i = 0; i < message.length; i++) {
        var song_entry = document.createElement('div');
        song_entry.className = "song-entry";
        song_entry.value = message[i].id;

        var entry_name_duration = document.createElement('div');
        entry_name_duration.className = "name-duration";
        var entry_name = document.createElement('p');
        entry_name.innerHTML = message[i].name;
        var entry_duration = document.createElement('p');
        if (message[i].duration < 10) {
            entry_duration.innerHTML = "0" + message[i].duration;
          } else {
            entry_duration.innerHTML = message[i].duration;
          }

        entry_name_duration.appendChild(entry_name);
        entry_name_duration.appendChild(entry_duration);

        var entry_artist_album = document.createElement('div');
        entry_artist_album.className = "artist-album";
        var entry_artist_album_text = document.createElement('p');
        entry_artist_album_text.innerHTML = message[i].artist + "&nbsp;&nbsp;&#183;&nbsp;&nbsp;" + message[i].album;

        entry_artist_album.appendChild(entry_artist_album_text);

        song_entry.appendChild(entry_name_duration);
        song_entry.appendChild(entry_artist_album);

        var plus = document.createElement('i');
        plus.id = "add";
        plus.className = "fas fa-plus fa-3x";

        document.getElementById("list-of-songs").removeChild(document.getElementById("list-of-songs").lastElementChild);
        document.getElementById("list-of-songs").appendChild(song_entry);

        document.getElementById("list-of-songs").appendChild(plus);
        document.getElementById("add").addEventListener("click", () => {
        document.getElementById("overlay").style.display = "block";
        document.getElementById("overlay").style.pointerEvents = "visible";
        });
    }
}

xhr.send();

document.getElementById("add").addEventListener("click", () => {
    document.getElementById("overlay").style.display = "block";
    document.getElementById("overlay").style.pointerEvents = "visible";
});

document.getElementById("overlay").addEventListener("click", (e) => {
  if(e.target.id == "overlay") {
    document.getElementById("overlay").style.display = "none";
    document.getElementById("overlay").style.pointerEvents = "none";
  }
});

document.getElementById("searchBtn").addEventListener("click", () => {
    var track = document.getElementById("searchTxt").value;
    let data = {track_name: track};

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/search-song");
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

    xhr.onloadend = function(e) {
        let resp = JSON.parse(xhr.responseText);
        let list_of_tracks = resp.tracks.items;

        document.getElementById("results").innerHTML = '';
        document.getElementById("results").style.padding = "0";
        document.getElementById("results").style.fontWeight = "normal";
        document.getElementById("results").style.alignItems = "baseline";

        var row = document.createElement('div');
        row.className = "row";

        var title_header = document.createElement('div');
        title_header.className = "title";
        var theader = document.createElement('h4');
        theader.innerHTML = "Title";
        title_header.appendChild(theader);

        var artist_header = document.createElement('div');
        artist_header.className = "artist";
        var aheader = document.createElement('h4');
        aheader.innerHTML = "Artist";
        artist_header.appendChild(aheader);

        var album_header = document.createElement('div');
        album_header.className = "album";
        var abheader = document.createElement('h4');
        abheader.innerHTML = "Album";
        album_header.appendChild(abheader);

        var blank_btn = document.createElement('btn');
        blank_btn.className = "blank-btn";
        blank_btn.innerHTML = "Add";
        blank_btn.style.color = "#fff";

        row.appendChild(title_header);
        row.appendChild(artist_header);
        row.appendChild(album_header);
        row.appendChild(blank_btn);
        document.getElementById("results").appendChild(row);

        for(var i = 0; i < list_of_tracks.length; i++) {
            var track_name = list_of_tracks[i].name;
            var album_name = list_of_tracks[i].album.name;
            var artists = list_of_tracks[i].artists[0].name;
            var spotify_id = list_of_tracks[i].id;

            if (list_of_tracks[i].artists.length != 1) {
                for(var j = 1; j < list_of_tracks[i].artists.length; j++) {
                    artists = artists + ", " + list_of_tracks[i].artists[j].name;
                }
            }

            var track_row = document.createElement('div');
            track_row.className = "row";

            var title = document.createElement('div');
            title.className = "title";
            var t = document.createElement('p');
            t.innerHTML = track_name;
            title.appendChild(t);

            var artist = document.createElement('div');
            artist.className = "artist";
            var a = document.createElement('p');
            a.innerHTML = artists;
            artist.appendChild(a);

            var album = document.createElement('div');
            album.className = "album";
            var ab = document.createElement('p');
            ab.innerHTML = album_name;
            album.appendChild(ab);

            var btn = document.createElement('btn');
            btn.className = "add-song";
            btn.innerHTML = "Add";
            btn.value = spotify_id;

            track_row.appendChild(title);
            track_row.appendChild(artist);
            track_row.appendChild(album);
            track_row.appendChild(btn);
            document.getElementById("results").appendChild(track_row);
        }

        let add_btns = document.querySelectorAll(".add-song");
        add_btns.forEach((btn) => {
          btn.addEventListener("click", () => {
            let trackId = { id: btn.value };
            var request = new XMLHttpRequest();
            request.open("POST", "/get-track");
            request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

            request.onloadend = function(e) {
              let response = JSON.parse(request.responseText);
              btn.innerHTML = "Added";

              var song_album = response.album.name;
              var song_artist = response.artists[0].name;
              var song_name = response.name;
              var song_duration_ms = response.duration_ms;
              var song_duration_min = Math.floor(song_duration_ms / 60000);
              var song_duration_sec = Math.floor(song_duration_ms / 1000) - 60 * song_duration_min;
              var song_duration = "0";
              if (song_duration_sec < 10) {
                song_duration = song_duration_min.toString() + ":0" + song_duration_sec.toString();
              } else {
                song_duration = song_duration_min.toString() + ":" + song_duration_sec.toString();
              }
              if (response.artists.length != 1) {
                for(var k = 1; k < response.artists.length; k++) {
                    song_artist = song_artist + ", " + response.artists[k].name;
                }
              }

              let connection_msg = {
                "type": "add-song",
                "album": song_album,
                "artist": song_artist,
                "name": song_name,
                "duration": song_duration,
                "id": response.id
              }

              let data = {
                  album: song_album,
                  artist: song_artist,
                  name: song_name,
                  duration: song_duration,
                  id: response.id
              }

              var xhr = new XMLHttpRequest();
              xhr.open("POST", "/update-playlist");
              xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
              xhr.onloadend = function () {};
              xhr.send(JSON.stringify(data));

              connection.send(JSON.stringify(connection_msg));
            }

            request.send(JSON.stringify(trackId));
          });
        });

    }

    xhr.send(JSON.stringify(data));
});

var first_song = true;
var song_is_playing = false;
connection.onmessage = event => {
  var message = JSON.parse(event.data);
  if(message.type == "client-connected") {
    var songs = document.getElementsByClassName("song-entry");
    if (songs.length > 0) {
      first_song = false;
      let data = {id: songs[0].value};
      document.getElementById("song").innerHTML = songs[0].firstChild.firstChild.innerHTML;
      document.getElementById("song-info").innerHTML = songs[0].childNodes[1].firstChild.innerHTML;
      document.getElementById("song").innerHTML = songs[0].firstChild.firstChild.innerHTML;
      document.getElementById("song-info").innerHTML = songs[0].childNodes[1].firstChild.innerHTML;
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/play");
      xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

      xhr.onloadend = function(e) {
        var update = setInterval(function() {check_progress( function (result) {
            var progress = result.progress;
            var percent = Math.floor(progress/total_time_s * 100);
            document.getElementById("progress-bar").style.width = percent + "%";
            var is_playing = result.is_playing;
            if(!is_playing) {
              clearInterval(update);
              document.getElementById("list-of-songs").removeChild(document.getElementById("list-of-songs").firstElementChild);
            }
        })}, 1000);
      }

      xhr.send(JSON.stringify(data));
    }
    
  }
  if(message.type == "add-song") {

    var song_entry = document.createElement('div');
    song_entry.className = "song-entry";
    song_entry.value = message.id;

    var entry_name_duration = document.createElement('div');
    entry_name_duration.className = "name-duration";
    var entry_name = document.createElement('p');
    entry_name.innerHTML = message.name;
    var entry_duration = document.createElement('p');
    entry_duration.innerHTML = message.duration;

    entry_name_duration.appendChild(entry_name);
    entry_name_duration.appendChild(entry_duration);

    var entry_artist_album = document.createElement('div');
    entry_artist_album.className = "artist-album";
    var entry_artist_album_text = document.createElement('p');
    entry_artist_album_text.innerHTML = message.artist + "&nbsp;&nbsp;&#183;&nbsp;&nbsp;" + message.album;

    entry_artist_album.appendChild(entry_artist_album_text);

    song_entry.appendChild(entry_name_duration);
    song_entry.appendChild(entry_artist_album);

    var plus = document.createElement('i');
    plus.id = "add";
    plus.className = "fas fa-plus fa-3x";

    document.getElementById("list-of-songs").removeChild(document.getElementById("list-of-songs").lastElementChild);
    document.getElementById("list-of-songs").appendChild(song_entry);

    document.getElementById("list-of-songs").appendChild(plus);
    document.getElementById("add").addEventListener("click", () => {
      document.getElementById("overlay").style.display = "block";
      document.getElementById("overlay").style.pointerEvents = "visible";
    });
  }
  if(message.type == "add-message" && message.id == name) {
    appendMessage(message.msg);
  }
  else if(message.type == "add-message"){
    appendOtherMessage(message.msg);
  }
}

const messageContainer = document.getElementById('message-container');
const messageForm = document.getElementById('send-container');
const messageInput = document.getElementById('message-input');
const name = Math.random().toString(36).substring(7);
const randomColor = Math.floor(Math.random()*16777215).toString(16);

messageForm.addEventListener('submit', e =>{
  e.preventDefault();
  const message = messageInput.value;
  let data = {
    "type": "add-message",
    "msg": message,
    "id": name,
    "color": randomColor
  };
  connection.send(JSON.stringify(data));
  messageInput.value = '';
})


function appendMessage(message){
  const messageElement = document.createElement ('div');
  const chatMessage = document.createElement('div');

  messageElement.setAttribute("id", "textBubble")
  chatMessage.setAttribute("id", "chatMessage")

  messageElement.style.marginLeft = "3rem";
  messageElement.style.backgroundColor = "#b3b3b3";
  messageElement.innerText = message;
  chatMessage.append(messageElement);
  messageContainer.append(chatMessage);
}

function appendOtherMessage(message){
  const chatOtherMessage = document.createElement('div');
  const messageElement = document.createElement ('div');
  const color = document.createElement('div');

  messageElement.setAttribute("id", "textBubble")
  color.setAttribute("id", "colorCircle")
  chatOtherMessage.setAttribute("id", "chatOtherMessage")

  color.style.backgroundColor = '#' + randomColor;
  messageElement.innerText = message;
  chatOtherMessage.append(color);
  chatOtherMessage.append(messageElement);
  messageContainer.append(chatOtherMessage);
}

document.getElementById("list-of-songs").addEventListener('DOMNodeInserted', (e) => {
  var songs = document.getElementsByClassName("song-entry");
  if(e.target.className == "song-entry") {
    if(first_song && songs.length == 1) {
      first_song = false;
      let data = {id: songs[0].value};
      document.getElementById("song").innerHTML = songs[0].firstChild.firstChild.innerHTML;
      document.getElementById("song-info").innerHTML = songs[0].childNodes[1].firstChild.innerHTML;
      var duration = songs[0].firstChild.childNodes[1].innerHTML.split(":");
      var total_time_s = Number(duration[0]) * 60 + Number(duration[1]); 
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/play");
      xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

      xhr.onloadend = function(e) {
        var update = setInterval(function() {check_progress( function (result) {
            var progress = result.progress;
            var percent = Math.floor(progress/total_time_s * 100);
            document.getElementById("progress-bar").style.width = percent + "%";
            var is_playing = result.is_playing;
            if(!is_playing) {
              clearInterval(update);
              document.getElementById("list-of-songs").removeChild(document.getElementById("list-of-songs").firstElementChild);
            }
        })}, 1000);
      }

      xhr.send(JSON.stringify(data));
    }
  }
});

function check_progress(callback) {
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/progress");
  xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

  xhr.onload = function() {
    var data = JSON.parse(xhr.responseText);
    var update = {};
    update.progress = Math.floor(data.progress_ms/1000);
    update.is_playing = data.is_playing;
    callback(update);
  }

  xhr.send();
}

document.getElementById("list-of-songs").addEventListener('DOMNodeRemoved', (e) => {
  if(e.target.className == "song-entry") {
    var songs = document.getElementsByClassName("song-entry");
    console.log(songs.length);
    if(songs.length-1 == 0) {
      first_song = true;
      console.log("reset");
    } else {
      let data = {id: songs[1].value};
      document.getElementById("song").innerHTML = songs[1].firstChild.firstChild.innerHTML;
      document.getElementById("song-info").innerHTML = songs[1].childNodes[1].firstChild.innerHTML;
      var duration = songs[0].firstChild.childNodes[1].innerHTML.split(":");
      var total_time_s = Number(duration[0]) * 60 + Number(duration[1]); 
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/play");
      xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
  
      xhr.onloadend = function(e) {
        var update = setInterval(function() {check_progress( function (result) {
          var progress = result.progress;
          var percent = Math.floor(progress/total_time_s * 100);
          document.getElementById("progress-bar").style.width = percent + "%";
          var is_playing = result.is_playing;
          if(!is_playing) {
            clearInterval(update);
            document.getElementById("list-of-songs").removeChild(document.getElementById("list-of-songs").firstElementChild);
          }
        })}, 1000);
      }
  
      xhr.send(JSON.stringify(data));
    }
  }
});

let open = document.getElementsByClassName("open-button")[0];
open.addEventListener("click", function() {
  console.log("open button works");
  document.getElementsByClassName("chat-popup")[0].style.display = "block";
});

let open2 = document.getElementsByClassName("open-button2")[0];
open2.addEventListener("click", function() {
  console.log("open button2 works");
  document.getElementsByClassName("chat-popup")[0].style.display = "block";
});

let close = document.getElementsByClassName("btn-cancel")[0];
close.addEventListener("click", function() {
  console.log("close button works");
  document.getElementsByClassName("chat-popup")[0].style.display = "none";
});

let overlay = document.getElementsByClassName('overlay')[0];
let popup = document.getElementsByClassName('popup')[0];
let exitButton = document.getElementsByClassName('exit');
let popupLink = document.getElementsByClassName('popupLink')[0];
document.getElementsByClassName('exit')[0].addEventListener('click', ()=> exit())

function exit(){
    overlay.style.display = 'none';
    popup.style.display = 'none';
    exitButton[0].style.display = 'none';
}
document.getElementsByClassName('invite')[0].addEventListener('click', ()=>share_invite());

function share_invite(){
  overlay.style.display = 'block';
  popup.style.display = 'block';
  exitButton[0].style.display = 'block';

  // change this based on your glitch
  popupLink.innerHTML = "https://wandering-temporal-foxtrot.glitch.me/client-login.html";
  popupLink.href = "https://wandering-temporal-foxtrot.glitch.me/client-login.html";
}
