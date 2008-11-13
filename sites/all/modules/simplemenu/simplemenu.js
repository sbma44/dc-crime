// $Id: simplemenu.js,v 1.9.2.12 2008/01/26 23:08:39 m3avrck Exp $

$(document).ready(function() {
  // get the Drupal basepath
  var basePath = Drupal.settings.simplemenu.basePath;
  // get the element to add the menu to
  var element = Drupal.settings.simplemenu.element;
  var menu = '<ul id="simplemenu" class="clear-block"></ul>';

  switch (Drupal.settings.simplemenu.placement) {
    case 'prepend':
      $(menu).prependTo(element);
      break;
    case 'append':
      $(menu).appendTo(element);
      break;
    case 'replace':
      $(element).html(menu);
      break;
  }

  $('body').addClass('simplemenu-enabled');

  var animation = {};
  animation[Drupal.settings.simplemenu.effect] = 'toggle';
  
  // Build menu
  $('#simplemenu')
    .append(simplemenu)
    .superfish( { 
      animation: animation,
      delay: Drupal.settings.simplemenu.hideDelay,
      speed: Drupal.settings.simplemenu.effectSpeed
    } )
  	.find(">li:has(ul)")
  		.mouseover(function(){
  			$("ul", this).bgIframe({opacity:false});
  		})
  		.find("a")
  			.focus(function(){
  				$("ul", $(".nav>li:has(ul)")).bgIframe({opacity:false});
  			})
  	  .end()
  	.end()
  	.find("a")
  	  .removeAttr('title');

	 $('#simplemenu').children('li.expanded').addClass('root');
});


/*
 * Superfish v1.3 - jQuery menu widget
 *
 * Copyright (c) 2007 Joel Birch
 *
 * Dual licensed under the MIT and GPL licenses:
 * 	http://www.opensource.org/licenses/mit-license.php
 * 	http://www.gnu.org/licenses/gpl.html
 *
 * YOU MAY DELETE THIS CHANGELOG:
 * v1.2.1 altered: 2nd July 07. added hide() before animate to make work for jQuery 1.1.3. See comment in 'over' function.
 * v1.2.2 altered: 2nd August 07. changed over function .find('ul') to .find('>ul') for smoother animations
 * 				   Also deleted the iframe removal lines - not necessary it turns out
 * v1.2.3 altered: jquery 1.1.3.1 broke keyboard access - had to change quite a few things and set display:none on the
 *				   .superfish rule in CSS instead of top:-999em
 * v1.3	: 		   Pretty much a complete overhaul to make all original features work in 1.1.3.1 and above.
 *				   .superfish rule reverted back to top:-999em (which is better)
 */

(function($){
	$.fn.superfish = function(o){
		var $sf = this,
			defaults = {
			hoverClass	: 'sfHover',
			pathClass	: 'overideThisToUse',
			delay		: 800,
			animation	: {opacity:'show'},
			speed		: 'normal'
		},
			over = function(){
				clearTimeout(this.sfTimer);
				clearTimeout($sf[0].sfTimer);
				$(this)
				.showSuperfishUl()
				.siblings()
				.hideSuperfishUl();
			},
			out = function(){
				var $$ = $(this);
				if ( !$$.is('.'+o.bcClass) ) {
					this.sfTimer=setTimeout(function(){
						$$.hideSuperfishUl();
						if (!$('.'+o.hoverClass,$sf).length) {
							over.call($currents.hideSuperfishUl());
						}
					},o.delay);
				}
			};
		$.fn.extend({
			hideSuperfishUl : function(){
				return this
					.removeClass(o.hoverClass)
					.find('ul:visible')
						.hide()
					.end();
			},
			showSuperfishUl : function(){
				return this
					.addClass(o.hoverClass)
					.find('>ul:hidden')
						.animate(o.animation,o.speed,function(){
							$(this).removeAttr('style');
						})
					.end();
			},
			applySuperfishHovers : function(){
				return this[($.fn.hoverIntent) ? 'hoverIntent' : 'hover'](over,out);
			}
		});
		o = $.extend({bcClass:'sfbreadcrumb'},defaults,o || {});
		var $currents = $('.'+o.pathClass,this).filter('li[ul]');
		if ($currents.length) {
			$currents.each(function(){
				$(this).removeClass(o.pathClass).addClass(o.hoverClass+' '+o.bcClass);
			});
		}
		var $sfHovAr=$('li[ul]',this).applySuperfishHovers(over,out)
			.find('a').each(function(){
				var $a = $(this), $li = $a.parents('li');
				$a.focus(function(){
					over.call($li);
					return false;
				}).blur(function(){
					$li.removeClass(o.hoverClass);
				});
			})
			.end()
			.not('.'+o.bcClass)
				.hideSuperfishUl()
			.end();
		$(window).unload(function(){
			$sfHovAr.unbind('mouseover').unbind('mouseout');
		});
		return this.addClass('superfish').blur(function(){
			out.call(this);
		});
	};
})(jQuery);


/* Copyright (c) 2006 Brandon Aaron (http://brandonaaron.net)
 * Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php)
 * and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 *
 * $LastChangedDate: 2007-07-21 18:45:56 -0500 (Sat, 21 Jul 2007) $
 * $Rev: 2447 $
 *
 * Version 2.1.1
 */
(function($){$.fn.bgIframe=$.fn.bgiframe=function(s){if($.browser.msie&&/6.0/.test(navigator.userAgent)){s=$.extend({top:'auto',left:'auto',width:'auto',height:'auto',opacity:true,src:'javascript:false;'},s||{});var prop=function(n){return n&&n.constructor==Number?n+'px':n;},html='<iframe class="bgiframe"frameborder="0"tabindex="-1"src="'+s.src+'"'+'style="display:block;position:absolute;z-index:-1;'+(s.opacity!==false?'filter:Alpha(Opacity=\'0\');':'')+'top:'+(s.top=='auto'?'expression(((parseInt(this.parentNode.currentStyle.borderTopWidth)||0)*-1)+\'px\')':prop(s.top))+';'+'left:'+(s.left=='auto'?'expression(((parseInt(this.parentNode.currentStyle.borderLeftWidth)||0)*-1)+\'px\')':prop(s.left))+';'+'width:'+(s.width=='auto'?'expression(this.parentNode.offsetWidth+\'px\')':prop(s.width))+';'+'height:'+(s.height=='auto'?'expression(this.parentNode.offsetHeight+\'px\')':prop(s.height))+';'+'"/>';return this.each(function(){if($('> iframe.bgiframe',this).length==0)this.insertBefore(document.createElement(html),this.firstChild);});}return this;};})(jQuery);