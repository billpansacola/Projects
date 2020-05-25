import java.awt.Point;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.Comparator;

public class AStarDiv_913735297 implements AIModule
{
    // Chebyshev distance
    private double cheb(final Point pt1, final Point pt2) 
    {
        return Math.max(Math.abs(pt2.x-pt1.x), Math.abs(pt2.y-pt1.y));
    }

    private double getHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
        // move diagonally toward the goal while climbing to get the lowest cost
        return cheb(pt1, pt2)/(2 * map.getTile(pt2)) - 1;
    }

    private ArrayList<Point> reconstruct(final HashMap<Point, Point> path, final Point endPoint)
    {
        ArrayList<Point> true_path = new ArrayList<Point>();
        true_path.add(0, endPoint);
        Point currPoint = endPoint;
        while(true)
        {
            if(path.containsKey(currPoint))
            {
                currPoint = path.get(currPoint);
                true_path.add(0, currPoint);
            }
            else
            {
                break;
            }
        }

        return true_path;
    }

    // Utilized https://en.wikipedia.org/wiki/A*_search_algorithm for AStar Psuedocode
    private ArrayList<Point> AStar(final TerrainMap map, final Point start, final Point end)
    {
        final HashMap<Point, Double> g = new HashMap<Point, Double>();
        g.put(start, 0.0);
        final HashMap<Point, Double> f = new HashMap<Point, Double>();
        f.put(start, getHeuristic(map, start, end));
        final HashMap<Point, Point> path = new HashMap<Point, Point>();
        final ArrayList<Point> empty_path = new ArrayList<Point>();
        PriorityQueue<Point> fringe = new PriorityQueue<Point>(map.getHeight() * map.getWidth(), new Comparator<Point>(){
            public int compare(final Point pt1, final Point pt2)
            {
                if (f.get(pt1) < f.get(pt2))
                {
                    return -1;
                }
                if (f.get(pt1) > f.get(pt2))
                {
                    return -1;
                }
                return 0;
            }
        });
        boolean[][] closed_set = new boolean[map.getWidth()][map.getHeight()];
        fringe.add(start);
        while(!fringe.isEmpty())
        {
            Point currPoint = fringe.poll();
            if(currPoint.x == end.x && currPoint.y == end.y)
            {
                return reconstruct(path, end);
            }
            if(!closed_set[currPoint.x][currPoint.y])
            {
                closed_set[currPoint.x][currPoint.y] = true;
                for(Point neighbor: map.getNeighbors(currPoint))
                {
                    if(closed_set[neighbor.x][neighbor.y])
                    {
                        continue;
                    }
                    double neighbor_gcost = g.get(currPoint) + map.getCost(currPoint, neighbor);
                    if(!g.containsKey(neighbor) || neighbor_gcost < g.get(neighbor))
                    {
                        path.put(neighbor, currPoint);
                        g.put(neighbor, neighbor_gcost);
                        f.put(neighbor, g.get(neighbor) + getHeuristic(map, neighbor, end));
                        if(!fringe.contains(neighbor))
                        {
                            fringe.add(neighbor);
                        }
                        else 
                        {
                            // add and remove neighbor node in order to be compared to any newly added nodes.
                            fringe.remove(neighbor);
                            fringe.add(neighbor);
                        }
                    }

                }
            }
        }
        return empty_path;
    }

    public List<Point> createPath(final TerrainMap map) 
    {
        final ArrayList<Point> path = AStar(map, map.getStartPoint(), map.getEndPoint());
        return path;
    }

}