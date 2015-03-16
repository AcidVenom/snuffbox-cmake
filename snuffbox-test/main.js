Game.targets = Game.targets || {
	gbuffer: new RenderTarget("G-Buffer"),
	normals: new RenderTarget("Normals"),
	light: new RenderTarget("Light"),
	default: new RenderTarget("Default")
}

var TestSphere = function(x, y, z)
{
	this._sphere = new Model("sphere.fbx");
	this._sphere.spawn("G-Buffer");
	this._sphere.setMaterial("test.material");

	this._light = new Light(LightType.Point);

	var r = Math.random();
	var g = Math.random();
	var b = Math.random();

	this._billboard = new Billboard(this._sphere);

	this._billboard.setMaterial("quad.material");
	this._billboard.setTechnique("Diffuse");
	this._billboard.spawn("Default");
	this._billboard.setOffset(0.5, 0.5);
	this._billboard.setBlend(r, g, b);

	this._light.setColour(r, g, b);
	this._light.setRadius(3);

	this._position = {x: x, y: y, z: z}

	this.update = function(t)
	{
		t *= 2;
		var r = 5.5;
		var x = this._position.x + Math.cos(t) * r;
		var y = this._position.y + Math.sin(t) * r;

		this._position.y = Math.sin(this._position.x + t) * 2;
		this._sphere.setTranslation(this._position.x, this._position.y, this._position.z);
		
		this._billboard.setTranslation(Math.cos(t) * r, Math.sin(t) * r);
		this._light.setTranslation(x, y, this._position.z);
	}
}

Game.Initialise = function()
{
	RenderSettings.setResolution(800, 600);
	ContentManager.load("box", "test.box");

	Game.camera = new Camera(CameraType.Perspective);

	Game.camera.setTranslation(0, 0, 0);
	Game.camera.setRotation(0, 0, 0);

	Game.camera.setNearPlane(1);
	Game.camera.setFarPlane(1000);

	Game.targets.gbuffer.setClearDepth(true);
	Game.targets.gbuffer.setLightingEnabled(true);
	Game.targets.gbuffer.addMultiTarget(Game.targets.normals);
	Game.targets.gbuffer.addMultiTarget(Game.targets.light);

	Game.targets.default.setClearDepth(false);
	Game.targets.default.setLightingEnabled(false);
	Game.targets.default.setPostProcessing("post_processing.effect");
	Game.targets.default.setTechnique("Diffuse");
	Game.targets.default.addMultiTarget(Game.targets.light);

	Game.light = new Light(LightType.Directional);
	Game.light.setDirection(0, -1, -1);

	Game.spheres = [];
	for (var x = 0; x < 20; ++x)
	{
		for (var y = 0; y < 20; ++y)
		{
			Game.spheres.push(new TestSphere(x * 12, 0, y * 12));
		}
	}

	Game.terrain = new Terrain();
	Game.terrain.spawn("G-Buffer");
	Game.terrain.setMaterial("test.material");
	Game.terrain.setTranslation(0, -9, 0);
	Game.terrain.setOffset(64, 0, 64);

	Game.text = new Text();
	Game.text.spawn("Default");
	Game.text.setFontSize(32);
	Game.text.setText("Dat vind ik niet netjes");
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}

	var mz = 0,
		mx = 0,
		rx = 0,
		ry = 0,
		speed = 20 * dt;

	if (Keyboard.isDown(Key.W))
	{
		mz = -speed;
	}
	else if (Keyboard.isDown(Key.S))
	{
		mz = speed;
	}

	if (Keyboard.isDown(Key.A))
	{
		mx = -speed;
	}
	else if (Keyboard.isDown(Key.D))
	{
		mx = speed;
	}

	if (Mouse.isDown(MouseButton.Left))
	{
		var movement = Mouse.movement();
		rx = movement.y / 100;
		ry = -movement.x / 100;
	}

	Game.camera.translateBy(mx, 0, mz, 0);
	Game.camera.rotateBy(rx, ry, 0);

	var time = Game.time();
	for (var i = 0; i < Game.spheres.length; ++i)
	{
		Game.spheres[i].update(time);
	}
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Draw = function(dt)
{
	Game.render(Game.camera, Game.targets.gbuffer);
	Game.render(Game.camera, Game.targets.default);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	
}
