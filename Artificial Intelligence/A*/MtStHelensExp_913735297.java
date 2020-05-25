import java.awt.Point;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.Comparator;
import java.util.Collections;

public class MtStHelensExp_913735297 implements AIModule
{
    // Chebyshev distance
    private double cheb(final Point pt1, final Point pt2) 
    {
        return Math.max(Math.abs(pt2.x-pt1.x), Math.abs(pt2.y-pt1.y));
    }

    private double height_difference(final TerrainMap map, final Point pt1, final Point pt2) 
    {
        return Math.abs(map.getTile(pt2) - map.getTile(pt2));
    }

    private double getHeuristic(final TerrainMap map, final Point pt1, final Point pt2)
    {
        if(map.getTile(pt1) < map.getTile(pt2))
        {
            return height_difference(map, pt1, pt2) * 2 + cheb(pt1, pt2);
        }
        if(map.getTile(pt1) > map.getTile(pt2))
        {
            return cheb(pt1, pt2)*Math.pow(2.0, -height_difference(map, pt1, pt2));
        }

        return cheb(pt1, pt2);
    }

    private ArrayList<Point> reconstruct(final HashMap<Point, Point> start_path, final HashMap<Point, Point> end_path, final Point midPoint)
    {   
        ArrayList<Point> start_to_mid = new ArrayList<Point>();
        start_to_mid.add(0, midPoint);
        Point currPoint = midPoint;
        while(true)
        {
            if(start_path.containsKey(currPoint))
            {
                currPoint = start_path.get(currPoint);
                start_to_mid.add(0, currPoint);
            }
            else
            {
                break;
            }
        }
        ArrayList<Point> mid_to_end = new ArrayList<Point>();
        mid_to_end.add(0, midPoint);
        currPoint = midPoint;
        while(true)
        {
            if(end_path.containsKey(currPoint))
            {
                currPoint = end_path.get(currPoint);
                mid_to_end.add(0, currPoint);
            }
            else
            {
                break;
            }
        }

        // have to reverse mid_to_end and then combine ArrayList
        Collections.reverse(mid_to_end);
        mid_to_end.remove(midPoint);
        
        start_to_mid.addAll(mid_to_end);

        return start_to_mid;
    }

    // Utilized https://en.wikipedia.org/wiki/A*_search_algorithm for AStar Psuedocode
    private ArrayList<Point> Bidirectional_AStar(final TerrainMap map, final Point start, final Point end)
    {
        final HashMap<Point, Double> g_start = new HashMap<Point, Double>();
        final HashMap<Point, Double> g_end = new HashMap<Point, Double>();
        g_start.put(start, 0.0);
        g_end.put(end, 0.0);
        final HashMap<Point, Double> f_start = new HashMap<Point, Double>();
        final HashMap<Point, Double> f_end = new HashMap<Point, Double>();
        f_start.put(start, getHeuristic(map, start, end));
        f_end.put(start, getHeuristic(map, end, start));
        final HashMap<Point, Point> start_path_from_prev = new HashMap<Point, Point>();
        final HashMap<Point, Point> end_path_from_prev = new HashMap<Point, Point>();
        final ArrayList<Point> empty_path = new ArrayList<Point>();
        PriorityQueue<Point> fringe_start = new PriorityQueue<Point>(map.getHeight() * map.getWidth(), new Comparator<Point>(){
            public int compare(final Point pt1, final Point pt2)
            {
                double pt1_f = f_start.get(pt1);
                double pt2_f = f_start.get(pt2);
                if (pt1_f < pt2_f)
                {
                    return -1;
                }
                if (pt1_f > pt2_f)
                {
                    return -1;
                }
                return 0;
            }
        });
        PriorityQueue<Point> fringe_end = new PriorityQueue<Point>(map.getHeight() * map.getWidth(), new Comparator<Point>(){
            public int compare(final Point pt1, final Point pt2)
            {
                double pt1_f = f_end.get(pt1);
                double pt2_f = f_end.get(pt2);
                if (pt1_f < pt2_f)
                {
                    return -1;
                }
                if (pt1_f > pt2_f)
                {
                    return -1;
                }
                return 0;
            }
        });
        boolean[][] start_closed_set = new boolean[map.getWidth()][map.getHeight()];
        boolean[][] end_closed_set = new boolean[map.getWidth()][map.getHeight()];
        fringe_start.add(start);
        fringe_end.add(end);
        while(!fringe_start.isEmpty() && !fringe_end.isEmpty())
        {
            Point start_currPoint = fringe_start.poll();
            Point end_currPoint = fringe_end.poll();
            if(start_currPoint.x == end_currPoint.x && start_currPoint.y == end_currPoint.y)
            {
                return reconstruct(start_path_from_prev, end_path_from_prev, start_currPoint);
            }
            if(!start_closed_set[start_currPoint.x][start_currPoint.y])
            {
                start_closed_set[start_currPoint.x][start_currPoint.y] = true;
                for(Point neighbor: map.getNeighbors(start_currPoint))
                {
                    if(start_closed_set[neighbor.x][neighbor.y])
                    {
                        continue;
                    }
                    double neighbor_gcost = g_start.get(start_currPoint) + map.getCost(start_currPoint, neighbor);
                    if(!g_start.containsKey(neighbor) || neighbor_gcost < g_start.get(neighbor))
                    {
                        start_path_from_prev.put(neighbor, start_currPoint);
                        g_start.put(neighbor, neighbor_gcost);
                        f_start.put(neighbor, g_start.get(neighbor) + getHeuristic(map, neighbor, end));
                        if(!fringe_start.contains(neighbor))
                        {
                            fringe_start.add(neighbor);
                        }
                        else 
                        {
                            // add and remove neighbor node in order to be compared to any newly added nodes.
                            fringe_start.remove(neighbor);
                            fringe_start.add(neighbor);
                        }
                    }

                }
            }
            if(!end_closed_set[end_currPoint.x][end_currPoint.y])
            {
                end_closed_set[end_currPoint.x][end_currPoint.y] = true;
                for(Point neighbor: map.getNeighbors(end_currPoint))
                {
                    if(end_closed_set[neighbor.x][neighbor.y])
                    {
                        continue;
                    }
                    double neighbor_gcost = g_end.get(end_currPoint) + map.getCost(end_currPoint, neighbor);
                    if(!g_end.containsKey(neighbor) || neighbor_gcost < g_end.get(neighbor))
                    {
                        end_path_from_prev.put(neighbor, end_currPoint);
                        g_end.put(neighbor, neighbor_gcost);
                        f_end.put(neighbor, g_end.get(neighbor) + getHeuristic(map, neighbor, start));
                        if(!fringe_end.contains(neighbor))
                        {
                            fringe_end.add(neighbor);
                        }
                        else 
                        {
                            // add and remove neighbor node in order to be compared to any newly added nodes.
                            fringe_end.remove(neighbor);
                            fringe_end.add(neighbor);
                        }
                    }

                }
            }
        }
        return empty_path;
    }

    public List<Point> createPath(final TerrainMap map) 
    {
        final ArrayList<Point> path = Bidirectional_AStar(map, map.getStartPoint(), map.getEndPoint());
        return path;
    }
}