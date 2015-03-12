Game.targets = Game.targets || {
	default: new RenderTarget("Default"),
	test: new RenderTarget("Test")
}

Game.Initialise = function()
{
	RenderSettings.setResolution(640, 480);
	ContentManager.load("box", "test.box");

	Game.camera = new Camera(CameraType.Perspective);

	Game.light = new Light(LightType.Directional);

	Game.camera.setTranslation(0, 0, 0);
	Game.camera.setRotation(0, 0, 0);

	Game.skybox = new Model("skybox.fbx");
	Game.skybox.spawn("Default");
	Game.skybox.setMaterial("test.material");
	Game.skybox.setTechnique("Skybox");

	Game.model = new Model("axew.fbx");
	Game.model.spawn("Default");

	Game.quad = new Quad();
	Game.quad.spawn("Default");

	Game.targets.default.addMultiTarget(Game.targets.test);
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

	var t = Game.camera.translation();
	Game.skybox.setTranslation(t.x, t.y, t.z);

	//Game.model.rotateBy(0, dt, 0);
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Draw = function(dt)
{
	Game.render(Game.camera, Game.targets.default);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	
}
