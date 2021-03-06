package com.example.superbreakout;

/**
 * The ball class contains all the properties of the ball object in the game.
 * It contains all movement related properties of the ball and the physics that come with it
 *
 * Code is based on the Pong game by Packt Publishing:
 * https://github.com/PacktPublishing/Learning-Java-by-Building-Android-Games-Second-Edition
 */

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.RectF;

import java.util.Random;

public class Ball extends GameObject {
    private RectF rect; // rectangle that represents the ball
    public double xVelocity; // horizontal component of velocity (positive in the right direction)
    public double yVelocity; // vertical component of velocity (positive in the downwards direction)
    private double speed; // speed of the ball with formula Math.sqrt(xVelocity^2 + yVelocity^2)
    public boolean explosion = false;
    public boolean hollow = false;

    private Bitmap ballBitmap;
    private BitmapDimensions bitmapDimensions; // specifies the dimensions of the bitmap image
    private int screenX;
    private int screenY;
    private boolean active;
    Context context;

    private int LEVEL_ONE_SPEED = 8000;
    private int LEVEL_TWO_SPEED = 9000;
    private int LEVEL_THREE_SPEED = 10000;
    private int LEVEL_FOUR_SPEED = 11000;
    private int LEVEL_FIVE_SPEED = 12000;

    // Make it a 60 pixel x 60 pixel square
    private static final float ballWidth = 10;
    private static final float ballHeight = 10;


    public Ball(Context context, int screenX, int screenY) {
        super(screenX/40, screenX/40);

        this.screenX = screenX;
        this.screenY = screenY;
        // Ball has issues here
        this.width = screenX/40;
        this.height = screenX/40;

        this.context = context;
        // creates new rectangle object for ball
        rect = new RectF();

        // width and height has to be added by these specific numbers to make ball look proportional
        bitmapDimensions = new BitmapDimensions(screenX/40, screenX/40);
        ballBitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.inactiveball);
        ballBitmap = Bitmap.createScaledBitmap(ballBitmap,
                bitmapDimensions.width, bitmapDimensions.height, true);
    }

    /* This function updates the movement of the ball
     *
     * @fps: frame rate at which the ball refreshes
     */

    public void update(long fps) {
        rect.left = rect.left + ((float)xVelocity / fps);
        rect.top = rect.top + ((float)yVelocity / fps);
        rect.right = rect.left + width;
        rect.bottom = rect.top + height;
    }

    /* This function sets the ball's velocity
     * at random Vy/Vx ratios and the magnitude
     * of such velocity depends on @level.
     *
     * @level: level of the current game
     */
    public void setRandomVelocity(int level) {
        switch (level) {
            case 1:
                this.setBallSpeed(LEVEL_ONE_SPEED);
                break;
            case 2:
                this.setBallSpeed(LEVEL_TWO_SPEED);
                break;
            case 3:
                this.setBallSpeed(LEVEL_THREE_SPEED);
                break;
            case 4:
                this.setBallSpeed(LEVEL_FOUR_SPEED);
                break;
            default:
                this.setBallSpeed(LEVEL_FIVE_SPEED);
                break;
        }

        int Vx, Vy; // Proposed horizontal and vertical components of velocity
        // randomly generate a variable that determines if the ball starts by moving left/right
        int xDirection = Randomizer.getRandNumber(1,3);
        if(xDirection >= 2)
            Vx = Randomizer.getRandNumber(4,8);
        else
            Vx = -Randomizer.getRandNumber(4,8);

        Vy = -Randomizer.getRandNumber(10,16); // Always start with upwards velocity

        this.normalizeVelocity(Vx, Vy); // Make velocity constant speed
    }

    /* This function checks if the @bat intersects the ball
     *
     * @bat: a bat object
     */
    public boolean intersect(Bat bat) {

        if (this.getRect().intersect(bat.getRect())) {
            return true;
        }
        if(RectF.intersects(bat.getRect(), this.getRect())) {
            return true;
        }
        if(bat.getRect().intersect(this.getRect())) {
            return true;
        }

        return false;
    }

    /* This function normalizes the velocity ratios
     * such that the speed is constant
     *
     * @Vx: horizontal velocity component ratio
     * @Vy: vertical velocity component ratio
     */
    public void normalizeVelocity(double Vx, double Vy) {
        double compensationFactor = this.speed / Math.sqrt((Vy*Vy + Vx*Vx));
        this.xVelocity = compensationFactor * Vx;
        this.yVelocity = compensationFactor * Vy;
    }

    /* This function is executed after the ball reflects the @bat
     * The new velocity of the ball depends on the distance between
     * the intersection point of the ball and the @bat and the midpoint
     * of the @bat.
     * Momentum is also added if the @bat is moving in the same horizontal direction
     * as the ball; momentum is likewise decreased if the @bat is moving in the
     * opposite horizontal direction as the ball.
     *
     * @fraction: (distance between point of intersection and bat) / (@bat length/2)
     * @bat: the bat object
     */
    public void getNewVelocity(float fraction, Bat bat) {
        // reflect y velocity but add interpolated factor of y velocity
        double newY = -this.yVelocity + fraction * this.yVelocity/2;
        // reflect x velocity, keeping it in the same direction and add interpolated factor of x velocity
        double newX =  this.xVelocity - fraction * this.xVelocity/2;

        // Momentum factor / spin
        if (bat.checkMovementStateRight()) {
            newX += bat.getPaddleSpeed()/5;
        }else if (bat.checkMovementStateLeft()){
            newX -= bat.getPaddleSpeed()/5;
        }

        this.clearObstacleY(bat.getRect().top);

        this.normalizeVelocity(newX, newY); // keep it at constant speed
    }

    // a fix for bug in Android RectF Class
    public void clearObstacleY(float y) {
        rect.bottom = y;
        rect.top = y - height;
    }

    // a fix for bug in Android RectF Class
    public void clearObstacleX(float x) {
        rect.left = x;
        rect.right = x + width;
    }

    /* This function resets the position of the ball and sets
     * the ball at random velocities based on the @level (speed)
     *
     * @x: x # pixels
     * @y: y # pixels
     * @level: current level of the game
     */
    public void reset(int x, int y, int level) {

        // Place the ball in the centre of the screen at the bottom
        rect.left = x / 2;
        rect.top = y - 200;
        rect.right = x / 2 + width;
        rect.bottom = y - 100 + height;

        this.setRandomVelocity(level);
    }

    public void setBallLocation(int x, int y, int level){
        // Place the ball in the centre of the screen at the bottom
        rect.left = x ;
        rect.top = y-200;
        rect.right = x + width;
        rect.bottom = y - 100 - height;

        this.setRandomVelocity(level);
        this.xVelocity = 0;
    }

    /**
     * @return Center of the ball.
     */
    public float getMiddle() {
        return (this.getRect().right - this.getRect().left) / 2;
    }

    /**
     * @return Rect object of the ball.
     */
    public RectF getRect() {
        return rect;
    }

    /**
     * @return Bitmap of the ball
     */
    // This function returns the bitmap image of the ball
    public Bitmap getBallBitmap() { return ballBitmap; }

    /**
     * This function reverses the vertical velocity and adds a little momentum to it.
     */
    public void reverseYVelocity() {
        yVelocity = -yVelocity + 50;
        this.normalizeVelocity(this.xVelocity, this.yVelocity);
    }

    /**
     * This function reverses the horizontal velocity and adds a little momentum to it.
     */
    public void reverseXVelocity() {
        if(xVelocity > 0) //collision on right wall
            xVelocity = -xVelocity - 50;
        else //collision on left wall
            xVelocity = -xVelocity + 50;
        this.normalizeVelocity(this.xVelocity, this.yVelocity);
    }

    /**
     * This function changes variables depending on upgrade
     *
     * @param upgradeName Type of upgrade.
     */
    public void applyUpgrade(String upgradeName) {
        switch(upgradeName) {
            case "Explosion":
                explosion = true;
                break;
            case "Slow":
                speed = speed/2;
                break;
        }
    }


    /**
     * This function changes variables depedning on downgrade
     *
     * @param downgradeName Type of downgrade.
     */
    public void applyDowngrade(String downgradeName) {

        switch(downgradeName) {
            case "Hollow":
                hollow = true;
                break;
        }
    }

    /**
     * Setter for ball speed.
     * Might be needed for additional features (upgrades or downgrades to ball speed).
     * @param ballSpeed Speed of the ball.
     */
    public void setBallSpeed(double ballSpeed) {
        this.speed = ballSpeed;
    }

    /**
     *
     * @return Speed of the ball.
     */
    public double getBallSpeed() { return this.speed; }

    /**
     *  Check if the ball hit the walls on the screen to change ball's trajectory
     */
    public void checkWallBounce(){
        // Bounce the ball back when it hits the top of screen
        if (getRect().top < 0) {
            reverseYVelocity();
            clearObstacleY(40);
        }

        // If the ball hits left wall bounce
        if (getRect().left < 0) {
            reverseXVelocity();
            clearObstacleX(2);
        }

        // If the ball hits right wall Velocity
        if (getRect().right > screenX) {
            reverseXVelocity();
            clearObstacleX(screenX - 57);
        }
    }

    /**
     * Checks if the ball hits the bat.
     * @param bat The Bat object.
     * @return True/False if the bat hits the ball.
     */
    public boolean checkBallBatCollision(Bat bat){
        // Check for ball colliding with paddle
        if(this.intersect(bat)) {
            if(!getActive()){
                makeActive();
                setRandomVelocity(3);
            }

            // Interpolate the incoming position for computation of the new Velocity
            float midBall = getMiddle();
            float midBat = bat.getMiddle();
            float fracDisplacementFromMid = (midBall - midBat) / midBat;

            getNewVelocity(fracDisplacementFromMid, bat);

            return true;
        }
        return false;
    }

    /**
     *
     * @return True/False if the ball goes out of the screen
     */
    public boolean checkMissBall(){
        if (getRect().bottom > screenY) return true;
        return false;
    }

    /**
     *
     * @return Status of the ball. Active colors the ball red, while inactive colors the ball grey.
     */
    public boolean getActive(){ return active;}

    /**
     * Makes the current ball object active.
     * Changes the bitmap of the ball object.
     */
    public void makeActive(){
        active =  true;
        
        // loads in asset and turns it into bitmaps
        ballBitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.ball);
        ballBitmap = Bitmap.createScaledBitmap(ballBitmap,
                bitmapDimensions.width, bitmapDimensions.height, true);
    }

    /**
     * If player misses the ball, ball stays deactivated.
     */
    public void playerMissedBall(){ active = false;}
}
