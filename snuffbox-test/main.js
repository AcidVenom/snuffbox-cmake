require("require");

Game.renderTarget = Game.renderTarget || new RenderTarget("Default");

timer = 0;
Game.Initialise = function()
{
	ContentManager.load("effect", "test.effect");
	ContentManager.load("effect", "pp.effect");
	ContentManager.load("texture", "wood.png");
	ContentManager.load("texture", "wood_normal.png");
	ContentManager.load("texture", "wood_bump.png");
	ContentManager.load("material", "diffuse.material");

	Game.renderTarget.setPostProcessing("pp.effect");
	
	Game.quad1 = new Quad();
	Game.quad1.spawn("Default");
	Game.quad1.setMaterial("diffuse.material");

	Game.quad2 = new Quad(Game.quad1);
	Game.quad2.spawn("Default");
	Game.quad2.setMaterial("diffuse.material");

	Game.quad3 = new Quad(Game.quad2);
	Game.quad3.spawn("Default");
	Game.quad3.setMaterial("diffuse.material");

	Game.camera = new Camera(CameraType.Perspective);
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}

	if (Keyboard.isDown(Key.W))
	{
		Game.camera.translateBy(0, 0, -dt * 10);
	}

	if (Keyboard.isDown(Key.S))
	{
		Game.camera.translateBy(0, 0, dt * 10);
	}

	if (Keyboard.isDown(Key.A))
	{
		Game.camera.translateBy(-dt * 10, 0, 0);
	}

	if (Keyboard.isDown(Key.D))
	{
		Game.camera.translateBy(dt * 10, 0, 0);
	}

	if (Mouse.isDown(MouseButton.Left))
	{
		var movement = Mouse.movement();
		Game.camera.rotateBy(movement.y / 100, -movement.x / 100, 0);
	}

	timer += dt*0.75;

	Game.quad1.setOffset(-0.5, -0.5, 0);
	Game.quad2.setTranslation(Math.cos(timer * 2) * 1, Math.sin(timer * 2) * 1, -0.5);
	Game.quad2.setBlend(0, 0, 1);
	Game.quad3.setOffset(-0.5, -0.5, 0);
	Game.quad2.setOffset(-0.5, -0.5, 0);
	Game.quad2.setRotation(0, 0, Math.sin(timer * 1.5) * Math.PI * 2);
	var s = 2 + Math.sin(timer * 3) / 2;
	Game.quad3.setScale(s, s, s);
	Game.quad3.setBlend(1, 0, 0);
	Game.quad3.setTranslation(0, 0, -0.5);
	Game.quad3.setRotation(0, 0, Math.sin(timer) * Math.PI);
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Draw = function(dt)
{
	Game.render(Game.camera);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	
}
