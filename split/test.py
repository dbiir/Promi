import torch
from torch.utils.tensorboard import SummaryWriter
from torch_geometric import transforms as T
from torch_geometric.datasets import Planetoid
from torch_geometric.nn import GCNConv, VGAE
from matplotlib import pyplot as plt
from sklearn.manifold import TSNE

def visualize(z):
    z=TSNE(n_components=2).fit_transform(z.detach().cpu().numpy())
    plt.figure(figsize=(10,10))
    plt.xticks([])
    plt.yticks([])
    plt.scatter(z[:,0], z[:,1], s=70, c=range(3327), cmap='Set2')
    plt.show()
##
device=torch.device('cuda' if torch.cuda.is_available() else 'cpu')
transform=T.Compose([
    T.NormalizeFeatures(),
    T.ToDevice(device),
    T.RandomLinkSplit(num_val=0.05, num_test=0.1, is_undirected=True,
                      split_labels=True, add_negative_train_samples=False)
])
dataset=Planetoid('./CiteSeer', name='CiteSeer', transform=transform)
train_data, val_data, test_data=dataset[0]
 
## encoder
class GCNencoder(torch.nn.Module):
    def __init__(self, in_channels, out_channels):
        super(GCNencoder, self).__init__()
        self.conv1=GCNConv(in_channels, 2*out_channels)
        self.con_mu=GCNConv(2*out_channels, out_channels)
        self.con_log=GCNConv(2*out_channels, out_channels)
    def forward(self, x, edge_index):
        x=self.conv1(x, edge_index)
        x=x.relu()
        mu=self.con_mu(x, edge_index)
        log=self.con_log(x, edge_index)
        return mu, log
## network
in_channels=dataset.num_features
print(in_channels)
out_channels=16
model=VGAE(GCNencoder(in_channels, out_channels))
model = model.to(device)
optimizer=torch.optim.Adam(model.parameters(), lr=0.01)
## train
def train():
    model.train()
    optimizer.zero_grad()
    z=model.encode(train_data.x, train_data.edge_index)
    loss=model.recon_loss(z, train_data.pos_edge_label_index)+1/train_data.num_nodes*model.kl_loss()
    loss.backward()
    optimizer.step()
    return loss
@torch.no_grad()
def test():
    model.eval()
    z=model.encode(test_data.x, test_data.edge_index)
    AUC, AP=model.test(z, test_data.pos_edge_label_index, test_data.neg_edge_label_index)
    return AUC, AP
epoch=100
writer=SummaryWriter('./logs')
for i in range(epoch):
    loss=train()
    AUC, AP=test()
    writer.add_scalar('loss', loss, i)
    writer.add_scalar('AUC', AUC, i)
    writer.add_scalar('AP', AP, i)
    print('epoch:{}, loss:{}, AUC:{}, AP:{}'.format(i+1, loss, AUC, AP))
writer.close()
## draw
z=model.encode(train_data.x, train_data.edge_index)
visualize(z)
 