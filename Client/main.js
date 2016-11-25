/**
 * Created by maiquel on 14/10/16.
 */


window.onload = function () {

	var host = "http://localhost:3001/";
	var remote_images = [];

	//var connection = new WebSocket(host, "standard");

	var xmlHttp = new XMLHttpRequest();
	xmlHttp.open("GET", host, true); // false for synchronous request


	xmlHttp.onload = function (data) {
		console.log(xmlHttp.response);
		remote_images = JSON.parse(xmlHttp.responseText);
		console.log(remote_images);
		render();
	};

	xmlHttp.send();



	function render() {
		var slides = [];

		/* JSON que vem do Server */
		// var remote_images = {
		// 	images: [
		// 		{
		// 			url: "http://2.bp.blogspot.com/_V9pEmPxa5zk/TLH3h8RfNhI/AAAAAAAAAF8/jjbbT503KKA/s1600/propagandas-engracadas-10.jpg",
		// 			title: "Natura",
		// 			caption: "Shampoo",
		// 			color: "rgb(255,0,0)"
		// 		},
		// 		{
		// 			url: "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg",
		// 			title: "Robô",
		// 			caption: "Feliz",
		// 			color: "#00F"
		// 		},
		// 		{
		// 			url: "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg",
		// 			title: "Outro robo",
		// 			caption: "Triste pq n demos nome",
		// 			color: "green"
		// 		}
		// 	],
		// 	messages: ["Outro robo lindo e querido","O Pritsch n acreditva que podia ter mais mensagens"],
		// 	city:1399
		// };

		/* Objeto Slide */
		function Slide(url, title, caption, color) {
			this.url = url;
			this.title = title;
			this.caption = caption;
			this.color = color;
		}

		/* Faz parse do JSON */
		remote_images.images.forEach(function (element) {
			var slide = new Slide(element.url, element.title, element.caption, element.color);
			console.log(element.color);
			slides.push(slide);
		});


		var slideshow = document.getElementById("slideshow");

		slides.forEach(function (slide) {
			var img =
				"<div class='slide'>" +
				"	<div class='title' style='color:" + slide.color + "' >" + slide.title + "</div>" +
				"	<img src='" + slide.url + "'>" +
				"	<div class='caption' style='color:" + slide.color + "' >" + slide.caption + "</div>" +
				"</div>";
			slideshow.insertAdjacentHTML("beforeend", img);
		});

		var remote_message = remote_images.messages.join("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-----&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");


		var message = document.getElementById("message");
		message.innerHTML = remote_message;


		/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


		var slides = document.getElementById('slideshow').children;						// pega todos os imgs
		var interval = 8000;															// duração do efeito
		var currentSlide = 0;
		slides[currentSlide].style.webkitAnimation = 'fadey ' + interval + 'ms';		// anima a primeira imagem
		slides[currentSlide].style.animation = 'fadey ' + interval + 'ms';
		var infiniteLoop = setInterval(function () {									// loo infinito
			slides[currentSlide].removeAttribute('style');								// remove o efeito da imagem atual
			if (currentSlide == slides.length - 1) {									// se foi a última, volta pra primeira
				currentSlide = 0;
			} else {
				currentSlide++;															// se não, avança a imagem
			}
			slides[currentSlide].style.webkitAnimation = 'fadey ' + interval + 'ms';    // adiciona efeito de animação na imagem atual
			slides[currentSlide].style.animation = 'fadey ' + interval + 'ms';
		}, interval);


	};
}

