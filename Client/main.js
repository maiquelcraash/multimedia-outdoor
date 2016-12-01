/**
 * Created by maiquel on 14/10/16.
 */


window.onload = function () {

	var host = "http://localhost:3001/";
	var remote_images = [];

	var xmlHttp = new XMLHttpRequest();
	xmlHttp.open("GET", host, true); // false for synchronous request


	xmlHttp.onload = function (data) {
		// console.log(xmlHttp.response);
		remote_images = JSON.parse(xmlHttp.responseText);
		// console.log(remote_images);
		renderSlides();
	};

	xmlHttp.send();

	getWather();


	function renderSlides() {
		var slides = [];

		/* Objeto Slide */
		function Slide(url, title, caption, color) {
			this.url = url;
			this.title = title;
			this.caption = caption;
			this.color = color;
		}

		/* Faz parse do JSON para objeto Slide */
		remote_images.images.forEach(function (element) {
			var slide = new Slide(element.url, element.title, element.caption, element.color);
			console.log(element.color);
			slides.push(slide);
		});

		/* Cria os componentes HTML para os slides */
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

		/* Configura mensagens no elemento e define a velocidade de execução de acordo com a quantidade de caracteres a uma taxa de 30 caracteres por segundo*/
		var remote_message = remote_images.messages.join("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-----&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
		var message = document.getElementById("message");
		message.innerHTML = remote_message;
		var duration = remote_message.length / 20;
		message.style.animation = "marquee " + duration + "s linear infinite";


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
	}

	function getWather(cityCode) {
		// var request = new XMLHttpRequest();
		//
		// request.open('GET', 'http://www.climatempo.com.br/previsao-do-tempo/cidade/1399', true);
		// request.setRequestHeader("Access-Control-Allow-Origin","*");
		// request.send();
		//
		// request.onload = function (source) {
		// 	// console.log(request.response);
		// 	console.log(source);
		// };

		// var x = document.getElementById("tempo");
		// var y = (x.contentWindow || x.contentDocument);
		// if (y.document) y = y.document;
		//
		// console.log(y);
		//
		// var weatherBox = y.getElementsByClassName("bg-box-prev-topo")[0];
		// y.body.innerHTML = weatherBox.innerHTML;
	}

};


