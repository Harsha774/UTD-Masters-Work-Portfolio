import torch
from torch.autograd import Variable
import torch.nn as nn
import torch.nn.functional as F

class SkipGramModel(nn.Module):
   def __init__(self, emb_size, emb_dimension):
      super(SkipGramModel, self).__init__()
      self.emb_size = emb_size
      self.emb_dimension = emb_dimension
      self.u_embeddings = nn.Embedding(emb_size, emb_dimension, sparse=True)
      self.v_embeddings = nn.Embedding(emb_size, emb_dimension, sparse = True)
      self.init_emb()
   def init_emb(self):
      initrange = 0.5 / self.emb_dimension
      self.u_embeddings.weight.data.uniform_(-initrange, initrange)
      self.v_embeddings.weight.data.uniform_(-0, 0)
   def forward(self, pos_u, pos_v, neg_v):
      emb_u = self.u_embeddings(pos_u)
      emb_v = self.v_embeddings(pos_v)
      score = torch.mul(emb_u, emb_v).squeeze()
      score = torch.sum(score, dim = 1)
      score = F.logsigmoid(score)
      neg_emb_v = self.v_embeddings(neg_v)
      neg_score = torch.bmm(neg_emb_v, emb_u.unsqueeze(2)).squeeze()
      neg_score = F.logsigmoid(-1 * neg_score)
      return -1 * (torch.sum(score)+torch.sum(neg_score))
   def save_embedding(self, id2word):
      embedding = self.u_embeddings.weight.data.numpy()
      fout = open('output.txt', 'w')
      fout.write('%d %d\n' % (len(id2word), self.emb_dimension))
      for wid, w in id2word.items():
         e = embedding[wid]
         e = ' '.join(map(lambda x: str(x), e))
         fout.write('%s %s\n' % (w, e))
def test():
   model = SkipGramModel(100, 100)
   id2word = dict()
   for i in range(100):
      id2word[i] = str(i)
   model.save_embedding(id2word)         

test()