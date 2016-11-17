/**
 * Created by maiquel on 14/10/16.
 */


window.onload = function () {
	/*	var remote_images = [
	 "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg",
	 "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg",
	 "https://pbs.twimg.com/profile_images/583194780039045121/LF6M0JEu.jpg"];*/

	var slides = [];


	/* JSON que vem do Server */
	var remote_images = {
		images: [
			{
				url: "https://pbs.twimg.com/profile_images/583194780039045121/LF6M0JEu.jpg",
				title: "Cachorro Quente",
				caption: "Comendo"
			},
			{
				url: "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/happy-bot-tinkerbot.jpg",
				title: "Robô",
				caption: "Feliz"
			},
			{
				url: "http://s3-us-west-2.amazonaws.com/s.cdpn.io/4273/dada-voltaire-tinkerbot.jpg",
				title: "Outro robo",
				caption: "Triste pq n demos nome"
			}
		]
	};

	/* Objeto Slide */
	function Slide(url, title, caption) {
		this.url = url;
		this.title = title;
		this.caption = caption;
	}

	/* Faz parse do JSON */
	remote_images.images.forEach(function (element) {
		var slide = new Slide(element.url, element.title, element.caption);
		slides.push(slide);
	});


	var slideshow = document.getElementById("slideshow");

	slides.forEach(function (slide) {
		var img =
			"<div class='slide'>" +
			"	<div class='title'>" + slide.title + "</div>" +
			"	<img src='" + slide.url + "'>" +
			"	<div class='caption'>" + slide.caption + "</div>" +
			"</div>";
		slideshow.insertAdjacentHTML("beforeend", img);
	});

	var remote_message = "Texto que vai de lá pra cá";

	var message = document.getElementById("message");
	message.innerHTML = remote_message;

	/*	function iframeRef(frameRef) {
	 return frameRef.contentWindow
	 ? frameRef.contentWindow.document
	 : frameRef.contentDocument
	 }

	 var tempoFrame = iframeRef(document.getElementById('tempo').firstElementChild);*/


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


