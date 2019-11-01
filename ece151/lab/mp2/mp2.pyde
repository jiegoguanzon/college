import random
import math

window_width = 1200
window_height = 800

graph_width = 700
graph_height = 700
graph_padding = window_height - graph_height

max_dim = 15
min_dim = 5
    
cols = rows = 15
dim_slider = 1100
spd_slider = 1100

inf = float("inf")

grid = [[0 for x in range(cols)] for y in range(rows)]
node_cost = [[inf for x in range(cols)] for y in range(rows)]
visited_nodes = [[0 for x in range(cols)] for y in range(rows)]

node_cost[0][0] = 0
visited_nodes[0][0] = 1

max_radius = 50
min_radius = 30

max_weight = 4
min_weight = 1

show_percentage = 40 + (max_dim / cols) + (max_dim / rows)

time = 6
temp = 0
stop = 0
dist = 0
start = 0
prev_node = 0
current_node = 0
connected_nodes = 0
number_of_visited_nodes = 0

def setup():

    size(window_width, window_height)
    init(0)

def draw():

    global start, stop, current_node, prev_node, temp, dist, grid
    global number_of_visited_nodes, connected_nodes, node_cost, visited_nodes
    global dim_slider, spd_slider, cols, rows, max_dim, min_dim, time
    
    if start == 0:
        if keyPressed:
            print(keyCode)
            if (key == CODED):
                if (keyCode == UP):
                    if (rows == max_dim):
                        pass
                    else: 
                        dim_slider = dim_slider + 30
                        cols = cols + 1
                        rows = rows + 1
                        init(0)
                elif (keyCode == DOWN):
                    if (rows == min_dim):
                        pass
                    else: 
                        dim_slider = dim_slider - 30
                        cols = cols - 1
                        rows = rows - 1
                        init(0)
                elif (keyCode == RIGHT):
                    spd_slider += 60
                    if spd_slider >= 1100:
                        spd_slider = 1100
                    time += 1
                    if time >= 6:
                        time = 6
                    init(1)
                elif (keyCode == LEFT):
                    spd_slider -= 60
                    if spd_slider <= 800:
                        spd_slider = 800
                    time -= 1
                    if time <= 1:
                        time = 1
                    init(1)
                elif (keyCode == SHIFT):
                    start = 1
                    frameRate(time)
                elif keyCode == CONTROL:
                    if stop == 1:
                        stop = 0
                        for i in range(100000):
                            pass
                        init(0)
    else:

        if stop == 1:
            frameRate(60)
            start = 0

        if start == 1:
            grid[0][0].cost = 0
            node_cost = [[inf for x in range(cols)] for y in range(rows)]
            visited_nodes = [[0 for x in range(cols)] for y in range(rows)]
            node_cost[0][0] = 0
            visited_nodes[0][0] = 1
            start_i = 0
            start_j = 0

            current_node = grid[start_i][start_j]
            current_node.shortest_path.append(grid[start_i][start_j])
            prev_node = grid[start_i][start_j]
            temp = grid[start_i][start_j]
            dist = 0

            number_of_visited_nodes = 1

            start = 2
            
        current_node = mindist()
        c_i = current_node.i
        c_j = current_node.j

        if visited_nodes[c_i][c_j] == 0:
            visited_nodes[c_i][c_j] = 1
            number_of_visited_nodes += 1
        
        c = 0
        dist = node_cost[c_i][c_j]

        for neighbor in current_node.neighbors:

            n_i = neighbor.i
            n_j = neighbor.j
            
            if visited_nodes[n_i][n_j] == 0:
                temp_cost = node_cost[c_i][c_j] + current_node.neighbor_costs[c]
                if node_cost[n_i][n_j] > temp_cost:
                    node_cost[n_i][n_j] = temp_cost
                    grid[n_i][n_j].cost = temp_cost;
                    neighbor.shortest_path = current_node.shortest_path[:]
                    neighbor.shortest_path.append(neighbor)
                    neighbor.predecessor = current_node
                node_cost[n_i][n_j] = min(node_cost[n_i][n_j], dist + current_node.neighbor_costs[c])
            
            c += 1

        if number_of_visited_nodes <= len(connected_nodes):
            background(192, 192, 192)

            for i in range(rows):
                for j in range(cols):
                    if grid[i][j] != 0:
                        grid[i][j].draw_edges()
                
            for i in range(rows):
                for j in range(cols):
                    if grid[i][j] != 0:
                        grid[i][j].show(255, 255, 255)
                        grid[i][j].print_cost(255, 0, 0)

            a = 0
            textAlign(CENTER, CENTER)

            if number_of_visited_nodes == len(connected_nodes):
                grid[rows - 1][cols - 1].show(0, 0, 255)
                grid[0][0].show(0, 255, 0)

                fill(255, 0, 0)
                for l in (grid[rows - 1][cols - 1].shortest_path):
                    l.show(255, 255, 51)
                    l.print_cost(255, 0, 0)
                    if (l.predecessor != 0):
                        stroke(255, 0, 0)
                        line(l.x, l.y, l.predecessor.x, l.predecessor.y)
                        l.predecessor.show(255, 255, 51)
                        l.predecessor.print_cost(255, 0, 0)
                    
                grid[0][0].show(0, 128, 255)
                grid[0][0].print_cost(255, 255, 255)

                grid[rows - 1][cols - 1].show(0, 128, 255)
                grid[rows - 1][cols - 1].print_cost(255, 255, 255)

                stop = 1
            else:
                current_node.show(0, 255, 0)
                fill(255, 0, 0)
                for l in (current_node.shortest_path):
                    l.show(255, 255, 51)
                    l.print_cost(255, 0, 0)
                    if (l.predecessor != 0):
                        stroke(255, 0, 0)
                        line(l.x, l.y, l.predecessor.x, l.predecessor.y)
                        l.predecessor.show(255, 255, 51)
                        l.predecessor.print_cost(255, 0, 0)
                    l.show(255, 255, 51)
                    l.print_cost(255, 0, 0)
                


def init(mode):

    textAlign(CENTER, CENTER)

    global connected_nodes

    if cols > rows:
        radius = 0.4 * graph_height / (cols - 1)
    else:
        radius = 0.4 * graph_height / (rows - 1)

    if radius > max_radius:
        radius = max_radius
    elif radius < min_radius:
        radius = min_radius

    while 1:

        background(192, 192, 192)

        textAlign(LEFT, CENTER)
        fill(255, 255, 255)
        textSize(25)
        message = "Number of Rows and Cols: %i" % (rows)
        text(message, 800, 100)
        textSize(12)
        message = "Use UP/DOWN arrow to increase/decrease size of grid"
        text(message, 800, 120)
        draw_slider(dim_slider, 800, 1100, 170)

        textSize(25)
        message = "Frames per Second: %i" % (time)
        text(message, 800, 230)
        textSize(12)
        message = "Use LEFT/RIGHT arrow to increase/decrease speed"
        text(message, 800, 250)
        draw_slider(spd_slider, 800, 1100, 300)

        textSize(25)
        message = "Press SHIFT to start."
        text(message, 800, 360)

        textSize(20)
        message = "Once done, press CTRL to reset."
        text(message, 800, 390)

        if mode == 1:
            break

        for i in range(rows):
            for j in range(cols):
                grid[i][j] = Node(i, j, radius)
                p = random.randint(1, 100)
                if p <= show_percentage or (i == 0 and j == 0) or (i == rows - 1 and j == cols - 1):
                    pass
                else:
                    grid[i][j] = 0
        
        for i in range(rows):
            for j in range(cols):
                if grid[i][j] != 0:
                    grid[i][j].add_neighbors()

        node_queue = []
        connected_nodes = []
        node_queue.append(grid[0][0])
        connected_nodes.append(grid[0][0])

        while node_queue != []:
            curr_node = node_queue.pop(0)
            for neighbor in curr_node.neighbors:
                if neighbor in connected_nodes:
                    continue
                node_queue.append(neighbor)
                connected_nodes.append(neighbor)
            
        if grid[rows - 1][cols - 1] in connected_nodes:
            break
        
    for i in range(rows):
        for j in range(cols):
            if grid[i][j] != 0:
                grid[i][j].draw_edges()
        
    for i in range(rows):
        for j in range(cols):
            if grid[i][j] != 0:
                grid[i][j].show(255, 255, 255)
                grid[i][j].print_cost(255, 0, 0)


def mindist():

    temp = grid[0][0]
    minimum = max(node_cost)

    for i in range(cols):
        for j in range(rows):
            if node_cost[i][j] < inf and node_cost[i][j] <= minimum and visited_nodes[i][j] == 0:
                minimum = node_cost[i][j]
                temp = grid[i][j]

    return temp

def draw_slider(slider, start_x, end_x, y):
    stroke(128,128,128)
    strokeWeight(5)
    line(start_x, y, end_x, y)
    fill(255)
    stroke(128,128,128)
    strokeWeight(3)
    ellipse(slider, y, 30, 30)

class Node: 

    def __init__(self, i, j, radius):
        self.i = i
        self.j = j
        self.x = self.j * graph_width / (cols - 1) + (graph_padding / 2)
        self.y = self.i * graph_height / (rows - 1) + (graph_padding / 2)
        self.radius = radius
        self.neighbors = []
        self.neighbor_costs = []
        self.shortest_path = []
        self.cost = inf
        self.predecessor = 0
    
    def show(self, red, blue, green):
        noStroke()
        fill(red, blue, green)
        ellipse(self.x, self.y, self.radius, self.radius)
    
    def add_neighbors(self):
        inc = 1
        while self.neighbors == []:

            for y in range(self.i - inc, self.i + inc + 1):
                for x in range(self.j - inc, self.j + inc + 1):
                    if y == self.i and x == self.j:
                        continue
                    if y >= 0 and x >= 0 and y < rows and x < cols:
                        if grid[y][x] != 0:
                            cost = random.randint(min_weight, max_weight + 1)
                            self.neighbors.append(grid[y][x])
                            grid[y][x].neighbors.append(grid[self.i][self.j])
                            self.neighbor_costs.append(cost)
                            grid[y][x].neighbor_costs.append(cost)

            inc += 1

    def draw_edges(self):
        i = 0
        textAlign(CENTER, CENTER)
        for neighbor in self.neighbors:
            midpoint_x = (self.x + neighbor.x) / 2
            midpoint_y = (self.y + neighbor.y) / 2
            stroke(128, 128, 128)
            line(self.x, self.y, neighbor.x, neighbor.y)
            fill(255, 255, 204)
            textSize(150 / (math.sqrt(cols) * 2))
            text(self.neighbor_costs[i], midpoint_x, midpoint_y)
            i += 1
    
    def print_cost(self, red, green, blue):
        textAlign(CENTER, CENTER)
        fill(red, green, blue)
        if self.cost < inf:
            text(self.cost, self.x, self.y - 2)
        else:
            text(self.cost, self.x - 4, self.y - 4)