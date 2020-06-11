const express = require("express");
const app = express();
const WebSocket = require('ws');

const bodyParser = require('body-parser');
const passport = require('passport');
const cookieParser = require('cookie-parser');
const expressSession = require('express-session');
const url = require('url');
const request = require("request");
const http = require("http");

app.use(expressSession(
    { 
      secret:'bananaBread',  // a random string used for encryption of cookies
      maxAge: 6 * 60 * 60 * 1000, // Cookie time out - six hours in milliseconds
      // setting these to default values to prevent warning messages
      resave: true,
      saveUninitialized: false,
      // make a named session cookie; makes one called "connect.sid" as well
      name: "ecs162-session-cookie"
}));
  
app.use(passport.initialize());
app.use(passport.session());
  
  // There are other strategies, including Facebook and Spotify
const SpotifyStrategy = require('passport-spotify').Strategy;

var tokens = {}

passport.use(
    new SpotifyStrategy(
      {
        clientID: "8b9f039568804ff783e1e850dff196fb",
        clientSecret: "fb072c92274b453fbc7b5d6bd56b647b",
        callbackURL: 'https://wandering-temporal-foxtrot.glitch.me/auth/spotify/callback',
        scope: ["user-read-playback-state", "user-modify-playback-state", "playlist-read-collaborative", "playlist-read-private", "user-read-playback-state", "user-read-currently-playing", "user-read-playback-state"]
      },
    // function to call once Passport gets the user's profile and accessToken from Spotify
    gotProfile
    )
);

// The first call to Passport, which redirects the login to Spotify to show the login menu
app.get("/auth/spotify", 
  function (req, res, next) {
    //console.log("At first auth");
    next();
  },   
  passport.authenticate('spotify'), function(req, res) {
  // The request will be redirected to spotify for authentication, so this
  // function will not be called and we don't have to return the HTTP response.
});

app.get(
    "/auth/spotify/callback",
    function (req, res, next) {
      //console.log("At second auth");
      next();
    },   
    passport.authenticate('spotify', { failureRedirect: '/login' }),
    function(req, res) {
      // Successful authentication, redirect home.
      if(req.headers.referer == "https://wandering-temporal-foxtrot.glitch.me/client-login.html") {
        res.redirect('/client-main.html')
      } else {
        res.redirect('/main.html');
      }
      // res.redirect('/main.html');
    }
);

app.use(bodyParser.json());
app.post("/search-song", function(req,res) {
  console.log(req.user);
  let accessToken = tokens[req.user];
  let track_name = req.body.track_name;
  if (track_name.search(" ")) {
    track_name = track_name.replace(/ /g, "%20");
  }
  var endpoint = "https://api.spotify.com/v1/search?q=" + track_name + "&type=track&market=US&limit=5";

  const options = {
    url: endpoint,
    method: 'GET',
    headers: {
      "Accept": `application/json`,
      "Content-Type": `application/json`,
      "Authorization": `Bearer ${accessToken}`
    }
  };

  request(options, 
    // The callback function for when Spotify responds
    (err, postres, body) => {
      if (err) {
          return console.log(err);
      }
      // console.log(`Status: ${postres.statusCode}`);
      // console.log(body);
      res.send(body);
      // just go back to the single homepage.  Later you might want to add a query string? 
      // or do this whole thing with an AJAX request rather than with redirects? 
  });
});

app.post("/get-track", function(req,res) {
  let accessToken = tokens[req.user];
  
  var endpoint = "https://api.spotify.com/v1/tracks/" + req.body.id + "?market=US";

  const options = {
    url: endpoint,
    method: 'GET',
    headers: {
      "Accept": `application/json`,
      "Content-Type": `application/json`,
      "Authorization": `Bearer ${accessToken}`
    }
  };

  request(options, 
    // The callback function for when Spotify responds
    (err, postres, body) => {
      if (err) {
          return console.log(err);
      }
      // console.log(`Status: ${postres.statusCode}`);
      // console.log(body);
      res.send(body);
      // just go back to the single homepage.  Later you might want to add a query string? 
      // or do this whole thing with an AJAX request rather than with redirects? 
  });
});

app.post("/play", function(req, res) {
  let accessToken = tokens[req.user];  // grab the user's access token
  
  // next, do an API call to Spotify at this URL
  let url = "https://api.spotify.com/v1/me/player/play";

  let body = {"uris": ["spotify:track:" + req.body.id]}

  const options = {
      url: url,
      json: true,
      body: body,
      headers: {
        "Accept": `application/json`,
        "Content-Type": `application/json`,
        "Authorization": `Bearer ${accessToken}`
      }
  };

  // send the PUT request!
  request.put(options, 
    // The callback function for when Spotify responds
    (err, postres, body) => {
      if (err) {
          return console.log(err);
      }
      // console.log(`Status: ${postres.statusCode}`);
      res.send();
      // res.redirect('/main.html');
      
  });
});

app.post("/progress", function(req,res) {

  let accessToken = tokens[req.user];
  
  var endpoint = "https://api.spotify.com/v1/me/player/currently-playing";

  const options = {
    url: endpoint,
    method: 'GET',
    headers: {
      "Accept": `application/json`,
      "Content-Type": `application/json`,
      "Authorization": `Bearer ${accessToken}`
    }
  };

  request(options, 
    // The callback function for when Spotify responds
    (err, postres, body) => {
      if (err) {
          return console.log(err);
      }
      // console.log(`Status: ${postres.statusCode}`);
      // console.log(body);
      res.send(body);
      // just go back to the single homepage.  Later you might want to add a query string? 
      // or do this whole thing with an AJAX request rather than with redirects? 
  });
});

function gotProfile(accessToken, refreshToken, expires_in, profile, done) {
    // the access tokens of all users are stored in the global object "tokens"
    tokens[profile.id] = accessToken;
    // console.log(profile);
    // it calls "done" to tell Passport to get on with whatever it was doing.
    // See login mini-lecture 25, the customization functions described around slide 7
    done(null, profile.id)
}

// profile.id was passed out of gotProfile and into and out of here 
passport.serializeUser(function(user, done) {
    done(null, user);
});

// profile.id was passed out of serializeUser, and into and out of here. Passport will put it into rec.user
passport.deserializeUser(function(user, done) {
    done(null, user);
});

// Usual static server stuff
app.get("/", (request, response) => {
  // console.log(request.user); // for debugging
  response.sendFile(__dirname + "/public/index.html");
});

// make all the files in 'public' available
app.use(express.static("public"));

const server = http.createServer(app);

const wss = new WebSocket.Server({server});

var currPlaylist = [];

app.get("/get-current-playlist", (req, res) => {
  res.send(currPlaylist);
});

app.post("/update-playlist", (req,res) => {
    currPlaylist.push(req.body);
    res.send();
});

wss.on('connection', (ws) => {
  ws.on('message', (message) => {
    // console.log(message)
    //ws.send("server echo:" + message);
    broadcast(message)
  })
})

function broadcast(data) {
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(data);
    }
  });
}

// listen for requests :)
// const listener = app.listen(process.env.PORT, () => {
//   console.log("Your app is listening on port " + listener.address().port);
// });

server.listen(process.env.PORT, () => {
  console.log(`Server started on port ${server.address().port} :)`);
});