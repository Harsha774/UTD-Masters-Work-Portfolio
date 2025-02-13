var express = require('express');
var router = express.Router();
var monk = require('monk');
var db = monk('localhost:27017/vidzy_db');
/* GET videos listing. */
router.get('/', function(req, res) {
	var collection = db.get('videos');
	collection.find({}, function(err, videos){
		if (err) throw err;
	  	res.json(videos);
	});
});

router.get('/:id', function(req, res) {
	var collection = db.get('videos');
	collection.findOne({_id:req.params.id}, function(err, videos){
		if (err) throw err;
	  	res.json(videos);
	});
});

module.exports = router;
