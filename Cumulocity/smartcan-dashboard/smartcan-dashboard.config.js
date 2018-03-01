(function () {
  'use strict';

  angular
    .module('cockpit-smartcan.smartcan-dashboard', [])
    .config(configure);

  /* @ngInject */
  function configure(
    c8yNavigatorProvider,
    c8yViewsProvider,
    gettext
  ) {
	  
	/* @ngInject */
	function DashController() {
		this.dashboardName = 'Smartcan Dashboard';
		this.isGlobal = false;
		this.children = [];
	}
	  
	  
    c8yNavigatorProvider.addNavigation({ // adds a menu item to the navigator with ...
      name: 'Smartcan Dashboard', // ... the name *"hello"*
      icon: 'tachometer', // ... the cube icon (icons are provided by the great Font Awesome library and you can use any of their [icon names](http://fontawesome.io/icons/) without the *fa-* prefix here
      priority: 100000, // ... a priority of 100000, which means that all menu items with a priority lower than 100000 appear before this menu item and all with a priority higher than 100000 appear after this menu item
      path: 'smartcandash' // ... */hello* as path
    });

    c8yViewsProvider.when('/smartcandash', { // when the path "/hello" is accessed ...
      //templateUrl: ':::PLUGIN_PATH:::/views/hello.html', //  ... display our html file "hello.html" inside the "views" folder of our plugin (the plugin's folder is represented using the magic string ```:::PLUGIN_PATH:::```, which is replaced by the actual path during the build process)
      template : '<c8y-dashboard-gridstack set-page-title="true" name="{{vm.dashboardName}}" default-children="vm.children" set-global="vm.isGlobal"></c8y-dashboard-gridstack>',
	  //controller: 'qu-mapcontroller', // ... use "HelloController" as controller
	  controller: DashController, // ... use "HelloController" as controller
      controllerAs: 'vm'
    });
  }
}());
