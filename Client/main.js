/**
 * Created by maiquel on 14/10/16.
 */


window.onload = function () {


	var host = "http://localhost:3001/";
	var server_response_current = {},
		server_response_newest = {};
	var infiniteLoop;

	init();

	//atualiza data-hora-temperatura a cada 2 segundos
	setInterval(function () {
		renderStatusNow();
	}, 2000);

	//atualiza slides a cada minuto, somente se houverem alterações
	setInterval(function () {
		init();
	}, (10 * 1000));

	function init() {
		var xmlHttp = new XMLHttpRequest();
		xmlHttp.open("GET", host, true); // false for synchronous request


		xmlHttp.onload = function (data) {
			server_response_newest = JSON.parse(xmlHttp.responseText);

			//valida se as informações provenientes do server são novas
			if (!deepCompare(server_response_newest, server_response_current)) {

				//valida se os slides são diferentes, desconsiderando a temperatura. Se for, atualiza os slides
				delete server_response_current.temperatura;
				delete server_response_newest.temperatura;
				if (!deepCompare(server_response_newest, server_response_current)) {
					server_response_current = JSON.parse(xmlHttp.responseText);
					renderSlides();
					console.log("Slides Atualizados");
				}

				//mesmo assim, atualiza a temperatura
				server_response_current = JSON.parse(xmlHttp.responseText);
				renderWeather();
				console.log("Temperatura Atualizada");
			}
			else {
				console.log("Sem dados novos");
			}
		};

		xmlHttp.send();
	}


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
		server_response_current.images.forEach(function (element) {
			var slide = new Slide(element.url, element.title, element.caption, element.color);
			console.log(element.color);
			slides.push(slide);
		});

		/* Cria os componentes HTML para os slides */
		var slideshow = document.getElementById("slideshow");
		slideshow.innerHTML = "";
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
		var remote_message = server_response_current.messages.join("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-----&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
		var message = document.getElementById("message");
		message.innerHTML = remote_message;
		var duration = remote_message.length / 20;
		message.style.animation = "marquee " + duration + "s linear infinite";



		/* Animação dos slides */
		var slides = document.getElementById('slideshow').children;						// pega todos os imgs
		var interval = 8000;															// duração do efeito
		var currentSlide = 0;
		slides[currentSlide].style.webkitAnimation = 'fadey ' + interval + 'ms';		// anima a primeira imagem
		slides[currentSlide].style.animation = 'fadey ' + interval + 'ms';

		clearTimeout(infiniteLoop);														// elimina animação anterior
		infiniteLoop = setInterval(function () {										// loop infinito
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

	function renderWeather() {
		/* formata o JSON para ser entendido como HTML */
		var parser = new DOMParser();
		var html = parser.parseFromString(server_response_current.weather, "text/html");
		html.innerHTML = server_response_current.weather;

		/* obtem os DOMs do html remoto */
		var condicao = html.getElementById("momento-condicao").textContent,
			sensacao = html.getElementById("momento-sensacao").textContent,
			humidade = html.getElementById("momento-humidade").textContent,
			pressao = html.getElementById("momento-pressao").textContent,
			tempo = document.getElementById("tempo");

		/* obtem os DOMs do documento local */
		var element_condicao = document.getElementById("condicao"),
			element_humidade = document.getElementById("humidade"),
			element_sensacao = document.getElementById("sensacao"),
			element_pressao = document.getElementById("pressao"),
			element_cidade = document.getElementById("cidade");

		/* atualiza a tela com as infos */
		element_condicao.textContent = condicao || "Offline";
		element_humidade.textContent = "Humidade: " + humidade;
		element_sensacao.textContent = "Sensação: " + sensacao;
		element_pressao.textContent = "Pressao: " + pressao;
		element_cidade.textContent = server_response_current.city;
	}

	function renderStatusNow() {
		/* obtém os html elements */
		var element_data = document.getElementById("data"),
			element_hora = document.getElementById("hora"),
			element_temperatura = document.getElementById("temperatura");

		/* formata a data e hora */
		var date = new Date(),
			day = date.getDate(),
			month = date.getMonth() + 1,
			year = date.getFullYear(),
			hour = date.getHours(),
			minutes = date.getMinutes();
		if (day < 10) day = "0" + day;
		if (month < 10) month = "0" + month;
		if (hour < 10) hour = "0" + hour;
		if (minutes < 10) minutes = "0" + minutes;

		/* formata a temperatura */
		var n_temp = Number(server_response_current.temperature),
			temperature = Math.floor(n_temp);

		/* atualiza os html components */
		element_data.textContent = day + "/" + month + "/" + year;
		element_hora.textContent = hour + ":" + minutes;
		element_temperatura.textContent = (temperature + " ºC");
	}

	/* função que compara objetos simples (só com propriedades, sem funções) */
	function deepCompare(o1, o2) {
		return JSON.stringify(o1) === JSON.stringify(o2);
	}
};

